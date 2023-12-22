// SPDX-License-Identifier: GPL-2.0+
/*
 * FB driver for the JD9853 LCD display controller
 *
 * This display uses 9-bit SPI: Data/Command bit + 8 data bits
 * For platforms that doesn't support 9-bit, the driver is capable
 * of emulating this using 8-bit transfer.
 * This is done by transferring eight 9-bit words in 9 bytes.
 *
 * Copyright (C) 2013 Christian Vogelgsang
 * Based on adafruit22fb.c by Noralf Tronnes
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <video/mipi_display.h>

#include "fbtft.h"

#define DRVNAME		"fb_jd9853"
#define WIDTH		240
#define HEIGHT		320
#define TXBUFLEN	(4 * PAGE_SIZE)
#define DEFAULT_GAMMA	"1F 1A 18 0A 0F 06 45 87 32 0A 07 02 07 05 00\n" \
			"00 25 27 05 10 09 3A 78 4D 05 18 0D 38 3A 1F"

static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);

	if (par->gpio.cs)
		gpiod_set_value(par->gpio.cs, 0);  /* Activate chip */

	// set password to access inhouse register
	write_reg(par, 0xDF, 0x98, 0x53);

	//set command page
	//page 0 command
	write_reg(par, 0xDE, 0x00);

	//Set VCOM voltage for normal scan direction
	//vcom=-0.3-0.025*value
	write_reg(par, 0xB2, 0x17);

	// Set positive / negative voltage of Gamma power
	write_reg(par, 0xB7, 0x00, 0x30, 0x00, 0x58);

	//Power mode and charge pump related setting
	write_reg(par, 0xBB, 0x4F, 0x9A, 0x55, 0x73, 0x6F, 0xF0);

	// Set Source Output driving ability
	write_reg(par, 0xC0, 0x22, 0x22);

	//Set Panel relate register
	//- - - SS_PANEL GS_PANEL REV_PANEL CFHR -
    //SS_Panel: Set Source scan output direction /1:S240-> S1  0:S1 -> S240
    //GS_Panel: Set Gate scan output direction /0:Top -> Bottom Scan (G1->G320)  1:Bottom -> Top Scan (G320 -> G1)
	//REV_Panel: Set the display of the same data on both normally-white
	//and normally-black panels. //0:Normal Black 1:Normal White
	//CFHR: Set color fliter horizontial alignment order  /1:BGR  0:RGB
	write_reg(par, 0xC1, 0x01); //0x12

	//Set Display Waveform Cycles of RGB Mode
	//- RGB_INV_PI[1:0] RGB_INV_I[1:0] IDLE_TYPE RGB_INV_NP[1:0]
	//RGB_INV_NP[1:0]: Set source dot inversion type at normal or partial mode /01:2-dot 10:Column 00:1-dot
	write_reg(par, 0xC3, 0x7D, 0x07, 0x14, 0x06, 0xC8, 0x71, 0x6C, 0x77);

	// Timing control setting
	//- - - - VBFP_RATIO[1:0] TE_OPT[1:0]
	//- TE_DELAY[6:0]
	//LN[7:0] : Sets the gate line number to drive LCD panel.Gate line number = LN*2 / 320  Line
	//SLT_NP[7:0]:Sets the scan line time width. (4 x OSC) CLK / step.Note: fosc = 10MHz
	//- VFP_NP[6:0]
	//VFP_xx[7:0]: Vertical front porch number setting. / 0x0E=60Hz 0x4E=50Hz 7E=45Hz
	write_reg(par, 0xC4, 0x00, 0x00, 0xA0, 0x79, 0x0E, 0x0A, 0x16, 0x79,
			 0x25, 0x0A, 0x16, 0x82);

	//Set Red Gamma output voltage.This command is used to set postive /neagative volatge of source output
	write_reg(par, 0xC8, 0x3F, 0x34, 0x2B, 0x20, 0x2A, 0x2C, 0x24, 0x24,
			 0x21, 0x22, 0x20, 0x15, 0x10, 0x0B, 0x06, 0x00, 0x3F,
			 0x34, 0x2B, 0x20, 0x2A, 0x2C, 0x24, 0x24, 0x21, 0x22,
			 0x20, 0x15, 0x10, 0x0B, 0x06, 0x00);
	//SET CGOUTx_L Signal Mapping, GS_Panel=0
	write_reg(par, 0xD0, 0x04, 0x06, 0x6B, 0x0F, 0x00);
	//RAMCTRL
	//- CR_OPTION SPI_2LAN_EN RP RM MLBIT_INV DM[1:0]
    //CR_OPTION: for data mapping.used with EPF[1:0]
    //SPI_2LAN_EN: Enable SPI 2 data lane when IM[3:0]=0101  //0=disable
	//RP : Enable DPI data path. 0=disable  1=enable
	//RM : select data path for GRAM. 1=data from DPI/DSI  0=data from 2C/3C command
	//MLBIT_INV: RGB data MSB/LSB reversal(only for MCU Interface RGB565,except QSPI)
	//DM[1:0]: select contol timing and display data path. 00=internal vs, hs ,de;Display Data Path=GRAM
	write_reg(par, 0xD7, 0x00, 0x30);

	write_reg(par, 0xE6, 0x14);

	//set command  page 1 command
	write_reg(par, 0xDE, 0x01);

	write_reg(par, 0xB7, 0x0C, 0x0C, 0x00, 0x33, 0x33);
	write_reg(par, 0xC1, 0x14, 0x15, 0xC0);
	write_reg(par, 0xC2, 0x06, 0x3A, 0xE7);
	write_reg(par, 0xC4, 0x72, 0x12);
	write_reg(par, 0xBE, 0x00);
	write_reg(par, 0xDE, 0x00);
	write_reg(par, 0x3A, 0x05);
	write_reg(par, 0x2A, 0x00, 0x00, 0x00, 0xEF);
	write_reg(par, 0x2B, 0x00, 0x00, 0x01, 0x3F);
	write_reg(par, 0x35, 0x00);
	write_reg(par, 0x11);
	mdelay(120);
	write_reg(par, 0x29);
	mdelay(20);
	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	write_reg(par, MIPI_DCS_SET_COLUMN_ADDRESS,
		  (xs >> 8) & 0xFF, xs & 0xFF, (xe >> 8) & 0xFF, xe & 0xFF);

	write_reg(par, MIPI_DCS_SET_PAGE_ADDRESS,
		  (ys >> 8) & 0xFF, ys & 0xFF, (ye >> 8) & 0xFF, ye & 0xFF);

	write_reg(par, MIPI_DCS_WRITE_MEMORY_START);
}

#define MEM_Y   BIT(7) /* MY row address order */
#define MEM_X   BIT(6) /* MX column address order */
#define MEM_V   BIT(5) /* MV row / column exchange */
#define MEM_L   BIT(4) /* ML vertical refresh order */
#define MEM_H   BIT(2) /* MH horizontal refresh order */
#define MEM_BGR (3) /* RGB-BGR Order */
static int set_var(struct fbtft_par *par)
{
	switch (par->info->var.rotate) {
	case 0:
		write_reg(par, MIPI_DCS_SET_ADDRESS_MODE,
			  MEM_X | (par->bgr << MEM_BGR));
		break;
	case 270:
		write_reg(par, MIPI_DCS_SET_ADDRESS_MODE,
			  MEM_V | MEM_L | (par->bgr << MEM_BGR));
		break;
	case 180:
		write_reg(par, MIPI_DCS_SET_ADDRESS_MODE,
			  MEM_Y | (par->bgr << MEM_BGR));
		break;
	case 90:
		write_reg(par, MIPI_DCS_SET_ADDRESS_MODE,
			  MEM_Y | MEM_X | MEM_V | (par->bgr << MEM_BGR));
		break;
	}

	return 0;
}

/*
 * Gamma string format:
 *  Positive: Par1 Par2 [...] Par15
 *  Negative: Par1 Par2 [...] Par15
 */
#define CURVE(num, idx)  curves[(num) * par->gamma.num_values + (idx)]
static int set_gamma(struct fbtft_par *par, u32 *curves)
{
	int i;

	for (i = 0; i < par->gamma.num_curves; i++)
		write_reg(par, 0xE0 + i,
			  CURVE(i, 0), CURVE(i, 1), CURVE(i, 2),
			  CURVE(i, 3), CURVE(i, 4), CURVE(i, 5),
			  CURVE(i, 6), CURVE(i, 7), CURVE(i, 8),
			  CURVE(i, 9), CURVE(i, 10), CURVE(i, 11),
			  CURVE(i, 12), CURVE(i, 13), CURVE(i, 14));

	return 0;
}

#undef CURVE

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = TXBUFLEN,
	.gamma_num = 2,
	.gamma_len = 15,
	.gamma = DEFAULT_GAMMA,
	.fbtftops = {
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.set_var = set_var,
		.set_gamma = set_gamma,
	},
};

FBTFT_REGISTER_DRIVER(DRVNAME, "jadard,jd9853", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:jd9853");
MODULE_ALIAS("platform:jd9853");

MODULE_DESCRIPTION("FB driver for the JD9853 LCD display controller");
MODULE_AUTHOR("Christian Vogelgsang");
MODULE_LICENSE("GPL");
