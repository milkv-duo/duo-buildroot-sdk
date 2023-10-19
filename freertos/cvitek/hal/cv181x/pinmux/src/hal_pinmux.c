//#include <stdint.h>
#include <string.h>
#include "top_reg.h"
#include "cv181x_pinmux.h"
#include "cv181x_reg_fmux_gpio.h"
#include "cv181x_pinlist_swconfig.h"
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
		case PINMUX_I2C3:
			PINMUX_CONFIG(IIC3_SCL, IIC3_SCL);
			PINMUX_CONFIG(IIC3_SDA, IIC3_SDA);
		break;
		case PINMUX_CAM0:
			PINMUX_CONFIG(CAM_RST0, XGPIOA_2);
			PINMUX_CONFIG(CAM_MCLK0, CAM_MCLK0);

		break;
		case PINMUX_CAM1:
			// PINMUX_CONFIG(CAM_RST0, XGPIOA_2);
			PINMUX_CONFIG(CAM_MCLK1, CAM_MCLK1);

		break;
		default:
			break;
	}
}
