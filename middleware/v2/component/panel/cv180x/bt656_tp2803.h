#ifndef _BT656_TP2803_H_
#define _BT656_TP2803_H_

#include <linux/cvi_comm_vo.h>

const VO_BT_ATTR_S stTP2803Cfg = {
	.pins = {
		.pin_num = 9,
		.d_pins = {
			{VO_VIVO_D0, VO_MUX_BT_DATA0},
			{VO_VIVO_D1, VO_MUX_BT_DATA1},
			{VO_VIVO_D2, VO_MUX_BT_DATA2},
			{VO_VIVO_D3, VO_MUX_BT_DATA3},
			{VO_VIVO_D4, VO_MUX_BT_DATA4},
			{VO_VIVO_D5, VO_MUX_BT_DATA5},
			{VO_VIVO_D6, VO_MUX_BT_DATA6},
			{VO_VIVO_D7, VO_MUX_BT_DATA7},
			{VO_VIVO_CLK, VO_MUX_BT_CLK}
		}
	},
};

#endif // _I80_PARAM_ST7789V_H_
