#ifndef _LVDS_PARAM_LCM185X56_H_
#define _LVDS_PARAM_LCM185X56_H_

#include <linux/cvi_comm_vo.h>

const VO_LVDS_ATTR_S lvds_lcm185x56_cfg = {
	.lvds_vesa_mode = VO_LVDS_MODE_JEIDA,
	.out_bits = VO_LVDS_OUT_8BIT,
	.chn_num = 1,
	.data_big_endian = 0,
	.lane_id = {VO_LVDS_LANE_0, VO_LVDS_LANE_1, VO_LVDS_LANE_CLK, VO_LVDS_LANE_2, VO_LVDS_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
};

#endif // _LVDS_PARAM_LCM185X56_H_
