#ifdef ENV_CVITEST
#include <common.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "system_common.h"
#include "timer.h"
#elif defined(ENV_EMU)
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "emu/command.h"
#else
#include "linux/types.h"
#include "linux/printk.h"
//#include <linux/delay.h>
#endif  // ENV_CVITEST

#include "vip_common.h"
#include "dwa_reg.h"
#include "dwa_cfg.h"
#include "cmdq.h"
#include "reg.h"

/****************************************************************************
 * Global parameters
 ***************************************************************************/
static uintptr_t reg_base;

/****************************************************************************
 * Initial info
 ***************************************************************************/
const u8 hcoeff_tap[COEFFICIENT_PHASE_NUM][4] = {
	{0x00, 0x40, 0x00, 0x00},
	{0xff, 0x40, 0x01, 0x00},
	{0xfe, 0x3f, 0x02, 0x01},
	{0xfe, 0x3f, 0x04, 0xff},
	{0xfd, 0x3e, 0x06, 0xff},
	{0xfc, 0x3c, 0x08, 0x00},
	{0xfc, 0x3b, 0x0a, 0xff},
	{0xfc, 0x39, 0x0c, 0xff},
	{0xfb, 0x38, 0x0f, 0xfe},
	{0xfb, 0x35, 0x11, 0xff},
	{0xfb, 0x33, 0x14, 0xfe},
	{0xfb, 0x31, 0x16, 0xfe},
	{0xfb, 0x2f, 0x19, 0xfd},
	{0xfb, 0x2c, 0x1c, 0xfd},
	{0xfc, 0x29, 0x1e, 0xfd},
	{0xfc, 0x27, 0x21, 0xfc},
	{0xfc, 0x24, 0x24, 0xfc},
	{0xfc, 0x21, 0x27, 0xfc},
	{0xfd, 0x1e, 0x29, 0xfc},
	{0xfd, 0x1c, 0x2c, 0xfb},
	{0xfd, 0x19, 0x2f, 0xfb},
	{0xfe, 0x16, 0x31, 0xfb},
	{0xfe, 0x14, 0x33, 0xfb},
	{0xfe, 0x11, 0x35, 0xfc},
	{0xfe, 0x0f, 0x38, 0xfb},
	{0xff, 0x0c, 0x39, 0xfc},
	{0xff, 0x0a, 0x3b, 0xfc},
	{0xff, 0x08, 0x3c, 0xfd},
	{0x00, 0x06, 0x3e, 0xfc},
	{0x00, 0x04, 0x3f, 0xfd},
	{0x00, 0x02, 0x3f, 0xff},
	{0x00, 0x01, 0x40, 0xff},
};
const u8 vcoeff_tap[COEFFICIENT_PHASE_NUM][4] = {
	{0x00, 0x40, 0x00, 0x00},
	{0xff, 0x40, 0x01, 0x00},
	{0xfe, 0x3f, 0x02, 0x01},
	{0xfe, 0x3f, 0x04, 0xff},
	{0xfd, 0x3e, 0x06, 0xff},
	{0xfc, 0x3c, 0x08, 0x00},
	{0xfc, 0x3b, 0x0a, 0xff},
	{0xfc, 0x39, 0x0c, 0xff},
	{0xfb, 0x38, 0x0f, 0xfe},
	{0xfb, 0x35, 0x11, 0xff},
	{0xfb, 0x33, 0x14, 0xfe},
	{0xfb, 0x31, 0x16, 0xfe},
	{0xfb, 0x2f, 0x19, 0xfd},
	{0xfb, 0x2c, 0x1c, 0xfd},
	{0xfc, 0x29, 0x1e, 0xfd},
	{0xfc, 0x27, 0x21, 0xfc},
	{0xfc, 0x24, 0x24, 0xfc},
	{0xfc, 0x21, 0x27, 0xfc},
	{0xfd, 0x1e, 0x29, 0xfc},
	{0xfd, 0x1c, 0x2c, 0xfb},
	{0xfd, 0x19, 0x2f, 0xfb},
	{0xfe, 0x16, 0x31, 0xfb},
	{0xfe, 0x14, 0x33, 0xfb},
	{0xfe, 0x11, 0x35, 0xfc},
	{0xfe, 0x0f, 0x38, 0xfb},
	{0xff, 0x0c, 0x39, 0xfc},
	{0xff, 0x0a, 0x3b, 0xfc},
	{0xff, 0x08, 0x3c, 0xfd},
	{0x00, 0x06, 0x3e, 0xfc},
	{0x00, 0x04, 0x3f, 0xfd},
	{0x00, 0x02, 0x3f, 0xff},
	{0x00, 0x01, 0x40, 0xff},

};

/****************************************************************************
 * Interfaces
 ****************************************************************************/

void dwa_set_base_addr(void *base)
{
	reg_base = (uintptr_t)base;
}

void dwa_disable(void)
{
	_reg_write(reg_base + REG_DWA_GLB_CTRL, 0x00000000);
}

/**
 * dwa_init - setup dwa, mainly interpolation settings.
 *
 * @param cfg: settings for this dwa's operation
 */
void dwa_init(void)
{
	uintptr_t i = 0;
	u8 interp_h_shift_num = 0, interp_v_shift_num = 0;
	union vip_sys_intr intr_mask;

	intr_mask = vip_get_intr_mask();
	intr_mask.b.dwa = 1;
	vip_set_intr_mask(intr_mask);

	// enable dwa
	_reg_write(reg_base + REG_DWA_GLB_CTRL, 0x00000001);

	if (INTERPOLATION_COEF_FBITS >= 5 && INTERPOLATION_COEF_FBITS <= 8) {
		interp_h_shift_num = INTERPOLATION_COEF_FBITS - 5;
		interp_v_shift_num = INTERPOLATION_COEF_FBITS - 5;
	} else {
		interp_h_shift_num = 0;
		interp_v_shift_num = 0;
	}
	_reg_write_mask(reg_base + REG_DWA_INTERP_OUTPUT_CTRL0, 0x00000330,
			(interp_v_shift_num<<8) | (interp_h_shift_num<<4));

	for (i = 0 ; i < COEFFICIENT_PHASE_NUM ; ++i) {
		_reg_write(reg_base + REG_DWA_INTERP_HCOEFF_PHASE0 + (i << 2),
			   (hcoeff_tap[i][3]<<24) | (hcoeff_tap[i][2]<<16) |
			   (hcoeff_tap[i][1]<<8) | hcoeff_tap[i][0]);
		_reg_write(reg_base + REG_DWA_INTERP_VCOEFF_PHASE0 + (i << 2),
			   (vcoeff_tap[i][3]<<24) | (vcoeff_tap[i][2]<<16) |
			   (vcoeff_tap[i][1]<<8) | vcoeff_tap[i][0]);
	}
}

/**
 * dwa_reset - do reset. This can be activated only if dma stop to avoid hang
 *	       fabric.
 *
 */
void dwa_reset(void)
{
	union vip_sys_reset mask;

	mask.raw = 0;
	mask.b.dwa = 1;
	vip_toggle_reset(mask);
}

/**
 * dwa_intr_ctrl - dwa's interrupt on(1)/off(0)
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void dwa_intr_ctrl(u8 intr_mask)
{
	_reg_write(reg_base + REG_DWA_INT_EN, intr_mask);
}

/**
 * dwa_intr_clr - clear dwa's interrupt
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void dwa_intr_clr(u8 intr_mask)
{
	_reg_write(reg_base + REG_DWA_INT_CLR, intr_mask);
}

/**
 * dwa_intr_status - dwa's interrupt status
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @return: The interrupt's status. 1 if active.
 */
u8 dwa_intr_status(void)
{
	return _reg_read(reg_base + REG_DWA_INT_STATUS);
}

/**
 * dwa_engine - start a dwa operation, wait frame_done intr after this.
 *              If output target is scaler, scaler's config should be done
 *              before this.
 *
 * @param cfg: settings for this dwa's operation
 */
void dwa_engine(struct dwa_cfg *cfg)
{
	// enable dwa
	//_reg_write(reg_base + REG_DWA_GLB_CTRL, 0x00000001);

	_reg_write(reg_base + REG_DWA_DATA_FORMAT,
		   (cfg->output_target<<4) | (cfg->pix_fmt&0x03));

	// mesh id addr
	_reg_write(reg_base + REG_DWA_MESH_ID_BASE_ADDR_LOW, cfg->mesh_id);
	_reg_write(reg_base + REG_DWA_MESH_ID_BASE_ADDR_HIGH,
		   (cfg->mesh_id) >> 32);

	// src buffer
	_reg_write(reg_base + REG_DWA_IMG_SRC_SIZE,
		   (cfg->src_height<<16) | cfg->src_width);

	_reg_write(reg_base + REG_DWA_R_Y_SRC_IMG_BASE_ADDR_LOW,
		   cfg->src_buf[R_IDX].addrl);
	_reg_write(reg_base + REG_DWA_R_Y_SRC_IMG_BASE_ADDR_HIGH,
		   cfg->src_buf[R_IDX].addrh);
	_reg_write(reg_base + REG_DWA_R_Y_SRC_PITCH, cfg->src_buf[R_IDX].pitch);
	_reg_write(reg_base + REG_DWA_R_Y_SRC_OFFSET,
		   (cfg->src_buf[R_IDX].offset_y<<16) |
		   cfg->src_buf[R_IDX].offset_x);

	_reg_write(reg_base + REG_DWA_G_U_SRC_IMG_BASE_ADDR_LOW,
		   cfg->src_buf[G_IDX].addrl);
	_reg_write(reg_base + REG_DWA_G_U_SRC_IMG_BASE_ADDR_HIGH,
		   cfg->src_buf[G_IDX].addrh);
	_reg_write(reg_base + REG_DWA_G_U_SRC_PITCH, cfg->src_buf[G_IDX].pitch);
	_reg_write(reg_base + REG_DWA_G_U_SRC_OFFSET,
		   (cfg->src_buf[G_IDX].offset_y<<16) |
		   cfg->src_buf[G_IDX].offset_x);

	_reg_write(reg_base + REG_DWA_B_V_SRC_IMG_BASE_ADDR_LOW,
		   cfg->src_buf[B_IDX].addrl);
	_reg_write(reg_base + REG_DWA_B_V_SRC_IMG_BASE_ADDR_HIGH,
		   cfg->src_buf[B_IDX].addrh);
	_reg_write(reg_base + REG_DWA_B_V_SRC_PITCH, cfg->src_buf[B_IDX].pitch);
	_reg_write(reg_base + REG_DWA_B_V_SRC_OFFSET,
		   (cfg->src_buf[B_IDX].offset_y<<16) |
		   cfg->src_buf[B_IDX].offset_x);

	// dst buffer
	_reg_write(reg_base + REG_DWA_IMG_DST_SIZE,
		   (cfg->dst_height<<16) | cfg->dst_width);

	_reg_write(reg_base + REG_DWA_R_Y_DST_IMG_BASE_ADDR_LOW,
		   cfg->dst_buf[R_IDX].addrl);
	_reg_write(reg_base + REG_DWA_R_Y_DST_IMG_BASE_ADDR_HIGH,
		   cfg->dst_buf[R_IDX].addrh);
	_reg_write(reg_base + REG_DWA_R_Y_DST_PITCH, cfg->dst_buf[R_IDX].pitch);
	_reg_write(reg_base + REG_DWA_R_Y_DST_OFFSET,
		   (cfg->dst_buf[R_IDX].offset_y<<16) |
		   cfg->dst_buf[R_IDX].offset_x);

	_reg_write(reg_base + REG_DWA_G_U_DST_IMG_BASE_ADDR_LOW,
		   cfg->dst_buf[G_IDX].addrl);
	_reg_write(reg_base + REG_DWA_G_U_DST_IMG_BASE_ADDR_HIGH,
		   cfg->dst_buf[G_IDX].addrh);
	_reg_write(reg_base + REG_DWA_G_U_DST_PITCH, cfg->dst_buf[G_IDX].pitch);
	_reg_write(reg_base + REG_DWA_G_U_DST_OFFSET,
		   (cfg->dst_buf[G_IDX].offset_y<<16) |
		   cfg->dst_buf[G_IDX].offset_x);

	_reg_write(reg_base + REG_DWA_B_V_DST_IMG_BASE_ADDR_LOW,
		   cfg->dst_buf[B_IDX].addrl);
	_reg_write(reg_base + REG_DWA_B_V_DST_IMG_BASE_ADDR_HIGH,
		   cfg->dst_buf[B_IDX].addrh);
	_reg_write(reg_base + REG_DWA_B_V_DST_PITCH, cfg->dst_buf[B_IDX].pitch);
	_reg_write(reg_base + REG_DWA_B_V_DST_OFFSET,
		   (cfg->dst_buf[B_IDX].offset_y<<16) |
		   cfg->dst_buf[B_IDX].offset_x);

	_reg_write(reg_base + REG_DWA_INTERP_OUTPUT_CTRL1, cfg->bgcolor);

	// start dwa
	_reg_write(reg_base + REG_DWA_GLB_CTRL, 0x00000003);

	// There should be a frame_down intr afterwards.
#ifdef ENV_EMU
	for (int i = 0 ; i < 0x400; i += 4) {
		if (i == 0x280)
			i = 0x300;
		else if (i == 0x380)
			break;
		printf("0x%x: 0x%x\n", i,
		       _reg_read(reg_base + i + REG_DWA_BASE));
	}
#endif
}

/**
 * dwa_engine_cmdq - start multiple dwa operation based on given cfgs, wait
 *		     cmdq-end intr after this. If output target is scaler,
 *		     scaler's config should be done before this.
 *
 * @param cfgs: settings for these dwa's operations
 * @param cnt: number of dwa operations
 */
void dwa_engine_cmdq(const void *cmdq_addr, struct dwa_cfg *cfgs, u8 cnt)
{
	u8 i = 0, cmd_idx = 0;
	union cmdq_set *cmd_start;

	cmd_start = (union cmdq_set *)cmdq_addr;

	memset(cmd_start, 0, sizeof(u32)*0x100);
	//_reg_write(reg_base + REG_DWA_GLB_CTRL, 0x00000001);
	for (i = 0; i < cnt; ++i) {
		struct dwa_cfg *cfg = (cfgs+i);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_DATA_FORMAT,
				 (cfg->output_target<<4) | (cfg->pix_fmt&0x03));

		// mesh id addr
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_MESH_ID_BASE_ADDR_LOW, cfg->mesh_id);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_MESH_ID_BASE_ADDR_HIGH,
				 (cfg->mesh_id) >> 32);

		// src buffer
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_IMG_SRC_SIZE,
				 (cfg->src_height<<16) | cfg->src_width);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_SRC_IMG_BASE_ADDR_LOW,
				 cfg->src_buf[R_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_SRC_IMG_BASE_ADDR_HIGH,
				 cfg->src_buf[R_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_SRC_PITCH,
				 cfg->src_buf[R_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_SRC_OFFSET,
				 (cfg->src_buf[R_IDX].offset_y<<16) |
				 cfg->src_buf[R_IDX].offset_x);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_SRC_IMG_BASE_ADDR_LOW,
				 cfg->src_buf[G_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_SRC_IMG_BASE_ADDR_HIGH,
				 cfg->src_buf[G_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_SRC_PITCH,
				 cfg->src_buf[G_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_SRC_OFFSET,
				 (cfg->src_buf[G_IDX].offset_y<<16) |
				 cfg->src_buf[G_IDX].offset_x);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_SRC_IMG_BASE_ADDR_LOW,
				 cfg->src_buf[B_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_SRC_IMG_BASE_ADDR_HIGH,
				 cfg->src_buf[B_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_SRC_PITCH,
				 cfg->src_buf[B_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_SRC_OFFSET,
				 (cfg->src_buf[B_IDX].offset_y<<16) |
				 cfg->src_buf[B_IDX].offset_x);

		// dst buffer
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_IMG_DST_SIZE,
				 (cfg->dst_height<<16) |
				 cfg->dst_width);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_DST_IMG_BASE_ADDR_LOW,
				 cfg->dst_buf[R_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_DST_IMG_BASE_ADDR_HIGH,
				 cfg->dst_buf[R_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_DST_PITCH,
				 cfg->dst_buf[R_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_R_Y_DST_OFFSET,
				 (cfg->dst_buf[R_IDX].offset_y<<16) |
				 cfg->src_buf[R_IDX].offset_x);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_DST_IMG_BASE_ADDR_LOW,
				 cfg->dst_buf[G_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_DST_IMG_BASE_ADDR_HIGH,
				 cfg->dst_buf[G_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_DST_PITCH,
				 cfg->dst_buf[G_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_G_U_DST_OFFSET,
				 (cfg->dst_buf[G_IDX].offset_y<<16) |
				 cfg->src_buf[G_IDX].offset_x);

		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_DST_IMG_BASE_ADDR_LOW,
				 cfg->dst_buf[B_IDX].addrl);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_DST_IMG_BASE_ADDR_HIGH,
				 cfg->dst_buf[B_IDX].addrh);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_DST_PITCH,
				 cfg->dst_buf[B_IDX].pitch);
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_B_V_DST_OFFSET,
				 (cfg->dst_buf[B_IDX].offset_y<<16) |
				 cfg->src_buf[B_IDX].offset_x);

		// start dwa
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_GLB_CTRL, 0x03);

		cmdQ_set_wait(&cmd_start[cmd_idx++], false, 0, 0x0);

		// clear intr
		cmdQ_set_package(&cmd_start[cmd_idx++].reg,
				 REG_DWA_INT_CLR, 0x05);
	}
	cmd_start[cmd_idx-1].reg.intr_end = 1;
	cmd_start[cmd_idx-1].reg.intr_last = 1;

	cmdQ_intr_ctrl(REG_DWA_CMDQ_BASE, 0x02);
	cmdQ_engine(REG_DWA_CMDQ_BASE, (uintptr_t)cmdq_addr, REG_DWA_BASE >> 22,
		    true, false, cmd_idx);
}

/**
 * dwa_is_busy - check if dwa's operation is finished.
 *              dwa_start can only be toggled if only dma done(frame_done intr),
 *              ow dma won't finished.
 */
bool dwa_is_busy(void)
{
	u32 cycles = _reg_read(reg_base + REG_DWA_FRAME_RUN_TIME);

	udelay(100);
	return (cycles != _reg_read(reg_base + REG_DWA_FRAME_RUN_TIME));
}
