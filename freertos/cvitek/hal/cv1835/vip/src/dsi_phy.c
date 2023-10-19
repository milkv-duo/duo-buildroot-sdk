#ifdef ENV_CVITEST
#include <common.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "system_common.h"
#elif defined(ENV_EMU)
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "emu/command.h"
#else
#include <sleep.h>
#include <delay.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/printk.h>
#include <linux/log2.h>
#endif  // ENV_CVITEST

#include "vip_common.h"
#include "scaler_reg.h"
#include "reg.h"
#include "dsi_phy.h"

/****************************************************************************
 * Global parameters
 ****************************************************************************/
static uintptr_t reg_base;
static u8 data_0_lane;

/****************************************************************************
 * Interfaces
 ****************************************************************************/
void dphy_set_base_addr(void *base)
{
	reg_base = (uintptr_t)base;
}

/**
 * dphy_dsi_lane_en - set dsi-lanes enable control.
 *                    setup before dphy_dsi_init().
 *
 * @param clk_en: clk lane enable
 * @param data_en: data lane[0-3] enable
 * @param preamble_en: preeamble enable
 */
void dphy_dsi_lane_en(bool clk_en, bool *data_en, bool preamble_en)
{
	u8 val = 0, i = 0;

	val |= clk_en;
	for (i = 0; i < 4; ++i)
		val |= (data_en[i] << (i+1));
	if (preamble_en)
		val |= 0x20;
	_reg_write_mask(reg_base + REG_DSI_PHY_EN, 0x3f, val);
}
EXPORT_SYMBOL_GPL(dphy_dsi_lane_en);

/**
 * dphy_dsi_set_lane - dsi-lanes control.
 *                     setup before dphy_dsi_lane_en().
 *
 * @param lane_num: lane[0-4].
 * @param lane: the role of this lane.
 * @param pn_swap: if this lane positive/negative swap.
 * @param clk_phase_shift: if this clk lane phase shift 90 degree.
 * @return: 0 for success.
 */
int dphy_dsi_set_lane(u8 lane_num, enum lane_id lane, bool pn_swap, bool clk_phase_shift)
{
	if ((lane_num > 4) || (lane > DSI_LANE_MAX))
		return -1;

	_reg_write_mask(reg_base + REG_DSI_PHY_LANE_SEL, 0x7 << (4 * lane_num), lane << (4 * lane_num));
	_reg_write_mask(reg_base + REG_DSI_PHY_LANE_PN_SWAP, BIT(lane_num), pn_swap << lane_num);

	if (lane == DSI_LANE_CLK)
		_reg_write_mask(reg_base + REG_DSI_PHY_LANE_SEL, 0x1f << 24,
				clk_phase_shift ? ((1 << 24) << lane_num) : 0);
	if (lane == DSI_LANE_0)
		data_0_lane = lane_num;

	return 0;
}
EXPORT_SYMBOL_GPL(dphy_dsi_set_lane);

/**
 * dphy_dsi_init - dphy init.
 *                 Invoked after dphy_dsi_set_lane() and dphy_dsi_lane_en().
 *
 */
void dphy_init(enum sclr_vo_intf intf)
{
	_reg_write(reg_base + REG_DSI_PHY_PD, (intf == SCLR_VO_INTF_MIPI || intf == SCLR_VO_INTF_LVDS)
		   ? 0x0 : 0x1f1f);
	_reg_write(reg_base + REG_DSI_PHY_ESC_INIT, 0x100);
	_reg_write(reg_base + REG_DSI_PHY_ESC_WAKE, 0x100);

	if ((intf == SCLR_VO_INTF_BT656) || (intf == SCLR_VO_INTF_BT1120) || (intf == SCLR_VO_INTF_I80))
		_reg_write(reg_base + REG_DSI_PHY_EXT_GPIO, 0x000fffff);
	else
		_reg_write(reg_base + REG_DSI_PHY_EXT_GPIO, 0x0);

	_reg_write(reg_base + REG_DSI_PHY_LVDS_EN, (intf == SCLR_VO_INTF_LVDS));
}

void _cal_pll_reg(u32 clkkHz, u32 VCORx10000, u32 *reg_txpll, u32 *reg_set, u32 factor)
{
	u8 gain = 1 << ilog2(MAX(1, 25000000UL / VCORx10000));
	u32 VCOCx1000 = VCORx10000 * gain / 10;
	u8 reg_disp_div_sel = VCOCx1000 / clkkHz;
	u8 dig_dig = ilog2(gain);
	u8 reg_divout_sel = MIN(3, dig_dig);
	u8 reg_div_sel = dig_dig - reg_divout_sel;
	u8 loop_gain = (((VCOCx1000 / 266000) + 7) >> 3) << 3;
	bool bt_div = reg_disp_div_sel > 0x7f;
	*reg_set = ((u64)(factor * loop_gain) << 26) / VCOCx1000;

	if (bt_div) {
		vip_sys_reg_write_mask(VIP_SYS_VIP_CLK_CTRL0, 0x10, 0);
		reg_disp_div_sel >>= 1;
	} else
		vip_sys_reg_write_mask(VIP_SYS_VIP_CLK_CTRL0, 0x10, 0x10);

	*reg_txpll = (reg_div_sel << 10) | (reg_divout_sel << 8) | reg_disp_div_sel;

	pr_debug("clkkHz(%d) VCORx10000(%d) gain(%d)\n", clkkHz, VCORx10000, gain);
	pr_debug("VCOCx1000(%d) dig_dig(%d) loop_gain(%d)\n", VCOCx1000, dig_dig, loop_gain);
	pr_debug("regs: disp_div_sel(%d), divout_sel(%d), set(%#x)\n", reg_disp_div_sel, reg_divout_sel, *reg_set);
	pr_debug("vip_sy : bt_div(%d)\n", bt_div);
}

void dphy_lvds_set_pll(u32 clkkHz, u8 link)
{
	u32 VCORx10000 = clkkHz * 70 / link;
	u32 reg_txpll, reg_set;

	_cal_pll_reg(clkkHz, VCORx10000, &reg_txpll, &reg_set, 1000000);

	_reg_write_mask(reg_base + REG_DSI_PHY_TXPLL, 0x7ff, reg_txpll);
	_reg_write(reg_base + REG_DSI_PHY_REG_SET, reg_set);
}

void dphy_dsi_set_pll(u32 clkkHz, u8 lane, u8 bits)
{
	u32 VCORx10000 = clkkHz * bits * 10 / lane;
	u32 reg_txpll, reg_set;

	_cal_pll_reg(clkkHz, VCORx10000, &reg_txpll, &reg_set, 1200000);

	_reg_write_mask(reg_base + REG_DSI_PHY_TXPLL, 0x7ff, reg_txpll);
	_reg_write(reg_base + REG_DSI_PHY_REG_SET, reg_set);

	// update
	_reg_write_mask(reg_base + REG_DSI_PHY_REG_8C, BIT(0), 0);
	_reg_write_mask(reg_base + REG_DSI_PHY_REG_8C, BIT(0), 1);
}
EXPORT_SYMBOL_GPL(dphy_dsi_set_pll);

#define dcs_delay 1

enum LP_DATA {
	LP_DATA_00 = 0x00010001,
	LP_DATA_01 = 0x00010101,
	LP_DATA_10 = 0x01010001,
	LP_DATA_11 = 0x01010101,
	LP_DATA_MAX
};

static inline void _data_0_manual_data(enum LP_DATA data)
{
	_reg_write(reg_base + REG_DSI_PHY_DATA_OV, data << data_0_lane);
	udelay(dcs_delay);
}

// LP-11, LP-10, LP-00, LP-01, LP-00
static void _esc_entry(void)
{
	_data_0_manual_data(LP_DATA_11);
	_data_0_manual_data(LP_DATA_10);
	_data_0_manual_data(LP_DATA_00);
	_data_0_manual_data(LP_DATA_01);
	_data_0_manual_data(LP_DATA_00);
}

// LP-00, LP-10, LP-11
static void _esc_exit(void)
{
	_data_0_manual_data(LP_DATA_00);
	_data_0_manual_data(LP_DATA_10);
	_data_0_manual_data(LP_DATA_11);
}

static void _esc_data(u8 data)
{
	u8 i = 0;

	for (i = 0; i < 8; ++i) {
		_data_0_manual_data(((data & (1 << i)) ? LP_DATA_10 : LP_DATA_01));
		_data_0_manual_data(LP_DATA_00);
	}
}

void dpyh_mipi_tx_manual_packet(const u8 *data, u8 count)
{
	u8 i = 0;

	_esc_entry();
	_esc_data(0x87); // LPDT
	for (i = 0; i < count; ++i)
		_esc_data(data[i]);
	_esc_exit();
	_reg_write(reg_base + REG_DSI_PHY_DATA_OV, 0x0);
}

void dphy_set_hs_settle(u8 prepare, u8 zero, u8 trail)
{
	_reg_write_mask(reg_base + REG_DSI_PHY_HS_CFG1, 0xffffff00, (trail << 24) | (zero << 16) | (prepare << 8));
}
EXPORT_SYMBOL_GPL(dphy_set_hs_settle);

void dphy_get_hs_settle(u8 *prepare, u8 *zero, u8 *trail)
{
	u32 value = _reg_read(reg_base + REG_DSI_PHY_HS_CFG1);

	if (prepare)
		*prepare = (value >> 8) & 0xff;
	if (zero)
		*zero = (value >> 16) & 0xff;
	if (trail)
		*trail = (value >> 24) & 0xff;
}
EXPORT_SYMBOL_GPL(dphy_get_hs_settle);
