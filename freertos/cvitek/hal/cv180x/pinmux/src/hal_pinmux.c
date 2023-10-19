//#include <stdint.h>
#include <string.h>
#include "top_reg.h"
#include "cv180x_pinmux.h"
#include "cv180x_reg_fmux_gpio.h"
#include "cv180x_pinlist_swconfig.h"
#include "pinctrl.h"
#include "hal_pinmux.h"
#include "mmio.h"

void hal_pinmux_config(int io_type)
{
	switch(io_type) {
		case PINMUX_UART0:
			PINMUX_CONFIG(UART0_RX, UART0_RX);
			PINMUX_CONFIG(UART0_TX, UART0_TX);
		break;
		case PINMUX_I2C0:
			PINMUX_CONFIG(IIC0_SCL, IIC0_SCL);
			PINMUX_CONFIG(IIC0_SDA, IIC0_SDA);
		break;
		case PINMUX_I2C2:
			#if defined(CV180X_QFN_68_PIN)
			PINMUX_CONFIG(PAD_MIPIRX1P, IIC1_SDA);
			PINMUX_CONFIG(PAD_MIPIRX0N, IIC1_SCL);
			#else
			PINMUX_CONFIG(PAD_MIPI_TXP1, IIC2_SCL);
			PINMUX_CONFIG(PAD_MIPI_TXM1, IIC2_SDA);
			#endif
		break;
		case PINMUX_CAM0:
			#if defined(CV180X_QFN_68_PIN)
			PINMUX_CONFIG(PAD_MIPIRX1N, XGPIOC_8);
			PINMUX_CONFIG(PAD_MIPIRX0P, CAM_MCLK0);
			#else
			PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
			PINMUX_CONFIG(PAD_MIPI_TXM0, CAM_MCLK1);
			#endif
		break;
		default:
			break;
	}
}
