#include <common.h>
#include <linux/delay.h>
#include "vip_common.h"
#include "scaler.h"
#include "scaler_reg.h"
#include "reg.h"
#include "dsi_phy.h"

/****************************************************************************
 * Global parameters
 ****************************************************************************/
static struct sclr_top_cfg g_top_cfg;
static struct sclr_disp_cfg g_disp_cfg;
static struct sclr_disp_timing disp_timing;
static uintptr_t reg_base;
/****************************************************************************
 * Initial info
 ****************************************************************************/
#define DEFINE_CSC_COEF0(a, b, c) \
		.coef[0][0] = a, .coef[0][1] = b, .coef[0][2] = c,
#define DEFINE_CSC_COEF1(a, b, c) \
		.coef[1][0] = a, .coef[1][1] = b, .coef[1][2] = c,
#define DEFINE_CSC_COEF2(a, b, c) \
		.coef[2][0] = a, .coef[2][1] = b, .coef[2][2] = c,
static struct sclr_csc_matrix csc_mtrx[SCL_CSC_MAX] = {
	// none
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		0)
		DEFINE_CSC_COEF1(0,		BIT(10),	0)
		DEFINE_CSC_COEF2(0,		0,		BIT(10))
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// yuv2rgb
	// 601 Limited
	//  R = Y + 1.402* Pr                           //
	//  G = Y - 0.344 * Pb  - 0.792* Pr             //
	//  B = Y + 1.772 * Pb                          //
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		1436)
		DEFINE_CSC_COEF1(BIT(10),	BIT(13) | 352,	BIT(13) | 731)
		DEFINE_CSC_COEF2(BIT(10),	1815,		0)
		.sub[0] = 0,   .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 601 Full
	//  R = 1.164 *(Y - 16) + 1.596 *(Cr - 128)                     //
	//  G = 1.164 *(Y - 16) - 0.392 *(Cb - 128) - 0.812 *(Cr - 128) //
	//  B = 1.164 *(Y - 16) + 2.016 *(Cb - 128)                     //
	{
		DEFINE_CSC_COEF0(1192,	0,		1634)
		DEFINE_CSC_COEF1(1192,	BIT(13) | 401,	BIT(13) | 833)
		DEFINE_CSC_COEF2(1192,	2065,		0)
		.sub[0] = 16,  .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 709 Limited
	// R = Y + 1.540(Cr – 128)
	// G = Y - 0.183(Cb – 128) – 0.459(Cr – 128)
	// B = Y + 1.816(Cb – 128)
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		1577)
		DEFINE_CSC_COEF1(BIT(10),	BIT(13) | 187,	BIT(13) | 470)
		DEFINE_CSC_COEF2(BIT(10),	1860,		0)
		.sub[0] = 0,   .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 709 Full
	//  R = 1.164 *(Y - 16) + 1.792 *(Cr - 128)                     //
	//  G = 1.164 *(Y - 16) - 0.213 *(Cb - 128) - 0.534 *(Cr - 128) //
	//  B = 1.164 *(Y - 16) + 2.114 *(Cb - 128)                     //
	{
		DEFINE_CSC_COEF0(1192,	0,		1836)
		DEFINE_CSC_COEF1(1192,	BIT(13) | 218,	BIT(13) | 547)
		DEFINE_CSC_COEF2(1192,	2166,		0)
		.sub[0] = 16,  .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// rgb2yuv
	// 601 Limited
	//  Y = 0.299 * R + 0.587 * G + 0.114 * B       //
	// Pb =-0.169 * R - 0.331 * G + 0.500 * B       //
	// Pr = 0.500 * R - 0.419 * G - 0.081 * B       //
	{
		DEFINE_CSC_COEF0(306,		601,		117)
		DEFINE_CSC_COEF1(BIT(13) | 173,	BIT(13) | 339,	512)
		DEFINE_CSC_COEF2(512,		BIT(13) | 429,	BIT(13) | 83)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 128, .add[2] = 128
	},
	// 601 Full
	//  Y = 16  + 0.257 * R + 0.504 * g + 0.098 * b //
	// Cb = 128 - 0.148 * R - 0.291 * g + 0.439 * b //
	// Cr = 128 + 0.439 * R - 0.368 * g - 0.071 * b //
	{
		DEFINE_CSC_COEF0(263,		516,		100)
		DEFINE_CSC_COEF1(BIT(13) | 152,	BIT(13) | 298,	450)
		DEFINE_CSC_COEF2(450,		BIT(13) | 377,	BIT(13) | 73)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 16,  .add[1] = 128, .add[2] = 128
	},
	// 709 Limited
	//   Y =       0.2126   0.7152   0.0722
	//  Cb = 128 - 0.1146  -0.3854   0.5000
	//  Cr = 128 + 0.5000  -0.4542  -0.0468
	{
		DEFINE_CSC_COEF0(218,		732,		74)
		DEFINE_CSC_COEF1(BIT(13) | 117,	BIT(13) | 395,	512)
		DEFINE_CSC_COEF2(512,		BIT(13) | 465,	BIT(13) | 48)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 128, .add[2] = 128
	},
	// 709 Full
	//  Y = 16  + 0.183 * R + 0.614 * g + 0.062 * b //
	// Cb = 128 - 0.101 * R - 0.339 * g + 0.439 * b //
	// Cr = 128 + 0.439 * R - 0.399 * g - 0.040 * b //
	{
		DEFINE_CSC_COEF0(187,		629,		63)
		DEFINE_CSC_COEF1(BIT(13) | 103,	BIT(13) | 347,	450)
		DEFINE_CSC_COEF2(450,		BIT(13) | 408,	BIT(13) | 41)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 16,  .add[1] = 128, .add[2] = 128
	},
};

/****************************************************************************
 * Interfaces
 ****************************************************************************/
void sclr_set_base_addr(void *base)
{
	reg_base = (uintptr_t)base;
}

/**
 * sclr_top_set_cfg - set scl-top's configurations.
 *
 * @param cfg: scl-top's settings.
 */
void sclr_top_set_cfg(struct sclr_top_cfg *cfg)
{
	u32 sc_top = 0;
	union vip_sys_clk clk;

	_reg_write(reg_base + REG_SCL_TOP_CFG0, cfg->ip_trig_src << 3);

	clk = vip_get_clk_lp();
	clk.b.sc_top = 1;
	vip_set_clk_lp(clk);

	if (cfg->sclr_enable[0])
		sc_top |= BIT(0);
	if (cfg->sclr_enable[1])
		sc_top |= BIT(1);
	if (cfg->sclr_enable[2])
		sc_top |= BIT(2);
	if (cfg->sclr_enable[3])
		sc_top |= BIT(3);
	if (cfg->disp_enable)
		sc_top |= BIT(4);
	if (cfg->disp_from_sc)
		sc_top |= BIT(8);
	if (cfg->sclr_d_src)
		sc_top |= BIT(9);

	_reg_write(reg_base + REG_SCL_TOP_CFG1, sc_top);
	sclr_img_set_trig(SCL_IMG_D, cfg->img_in_d_trig_src);
	sclr_img_set_trig(SCL_IMG_V, cfg->img_in_v_trig_src);

	sclr_top_reg_done();
	sclr_top_reg_force_up();
	g_top_cfg = *cfg;
}

/**
 * sclr_top_get_cfg - get scl_top's cfg
 *
 * @return: scl_top's cfg
 */
struct sclr_top_cfg *sclr_top_get_cfg(void)
{
	return &g_top_cfg;
}

/**
 * sclr_top_reg_done - to mark all sc-reg valid for update.
 *
 */
void sclr_top_reg_done(void)
{
	_reg_write_mask(reg_base + REG_SCL_TOP_CFG0, 0, 1);
}

/**
 * sclr_top_reg_force_up - trigger reg update by sw.
 *
 */
void sclr_top_reg_force_up(void)
{
	_reg_write_mask(reg_base + REG_SCL_TOP_SHD, 0x00ff, 0xff);
}

u8 sclr_top_pg_late_get_bus(void)
{
	return (_reg_read(reg_base + REG_SCL_TOP_PG) >> 8) & 0xff;
}

void sclr_top_pg_late_clr(void)
{
	_reg_write_mask(reg_base + REG_SCL_TOP_PG, 0x0f0000, 0x80000);
}

union sclr_intr sclr_get_intr_mask(void)
{
	union sclr_intr intr_mask;

	intr_mask.raw = _reg_read(reg_base + REG_SCL_TOP_INTR_MASK);
	return intr_mask;
}

/**
 * sclr_set_intr_mask - sclr's interrupt mask. Only enable ones will be
 *			integrated to vip_subsys.  check 'union sclr_intr'
 *			for each bit mask.
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void sclr_set_intr_mask(union sclr_intr intr_mask)
{
	_reg_write(reg_base + REG_SCL_TOP_INTR_MASK, intr_mask.raw);
}

/**
 * sclr_intr_clr - clear sclr's interrupt
 *                 check 'union sclr_intr' for each bit mask
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void sclr_intr_clr(union sclr_intr intr_mask)
{
	_reg_write(reg_base + REG_SCL_TOP_INTR_STATUS, intr_mask.raw);
}

/**
 * sclr_intr_status - sclr's interrupt status
 *                    check 'union sclr_intr' for each bit mask
 *
 * @return: The interrupt's status
 */
union sclr_intr sclr_intr_status(void)
{
	union sclr_intr status;

	status.raw = (_reg_read(reg_base + REG_SCL_TOP_INTR_STATUS) & 0xffff);
	return status;
}

/**
 * sclr_img_set_trig - set img's src of job_start.
 *
 * @param inst: (0~1), the instance of img-in which want to be configured.
 * @param trig_src: img's src of job_start.
 */
void sclr_img_set_trig(u8 inst, enum sclr_img_trig_src trig_src)
{
	u32 mask = BIT(12) | BIT(8);
	u32 val = 0;

	switch (trig_src) {
	case SCL_IMG_TRIG_SRC_SW:
		break;
	case SCL_IMG_TRIG_SRC_DISP:
		val |= BIT(8);
		break;
	case SCL_IMG_TRIG_SRC_ISP:
		val |= BIT(12);
		break;
	default:
		break;
	}

	if (inst == SCL_IMG_V) {
		mask <<= 1;
		val <<= 1;
		g_top_cfg.img_in_v_trig_src = trig_src;
	} else {
		g_top_cfg.img_in_d_trig_src = trig_src;
	}
	//uartlog("mask(%#x) val(%#x)\n", mask, val);
	_reg_write_mask(reg_base + REG_SCL_TOP_IMG_CTRL, mask, val);
}

/****************************************************************************
 * SCALER GOP
 ****************************************************************************/
/**
 * sclr_gop_set_cfg - configure gop
 *
 * @param inst: (0~4), the instance of gop which want to be configured.
 *		0~3 is on scl, 4 is on disp.
 * @param cfg: gop's settings
 */
void sclr_gop_set_cfg(u8 inst, struct sclr_gop_cfg *cfg)
{
	_reg_write(reg_base + REG_SCL_DISP_GOP_CFG, cfg->raw & 0xffff);
	_reg_write(reg_base + REG_SCL_DISP_GOP_FONTCOLOR,
		   (cfg->font_fg_color << 16) | cfg->font_bg_color);
	if (cfg->b.colorkey_en)
		_reg_write(reg_base + REG_SCL_DISP_GOP_COLORKEY,
			   cfg->colorkey);

	g_disp_cfg.gop_cfg = *cfg;
}

/**
 * sclr_gop_get_cfg - get gop's configurations.
 *
 * @param inst: (0~4), the instance of gop which want to be configured.
 *		0~3 is on scl, 4 is on disp.
 */
struct sclr_gop_cfg *sclr_gop_get_cfg(u8 inst)
{
	return &g_disp_cfg.gop_cfg;

	return NULL;
}

/**
 * sclr_gop_setup LUT - setup gop's Look-up table
 *
 * @param inst: (0~4), the instance of gop which want to be configured.
 *		0~3 is on scl, 4 is on disp.
 * @param data: values of LUT-table. There should be 256 instances.
 */
void sclr_gop_setup_LUT(u8 inst, u16 *data)
{
	u16 i = 0;

	for (i = 0; i < 256; ++i) {
		_reg_write(reg_base + REG_SCL_DISP_GOP_LUT0,
			   (i << 16) | *(data + i));
		_reg_write(reg_base + REG_SCL_DISP_GOP_LUT1, (u32)~BIT(16));
		_reg_write(reg_base + REG_SCL_DISP_GOP_LUT1, BIT(16));
	}
}

/**
 * sclr_gop_update_LUT - update gop's Look-up table by index.
 *
 * @param inst: (0~4), the instance of gop which want to be configured.
 *		0~3 is on scl, 4 is on disp.
 * @param index: start address of LUT-table. There should be 256 instances.
 * @param data: value of LUT-table.
 */
int sclr_gop_update_LUT(u8 inst, u8 index, u16 data)
{
	if (index > 255)
		return -1;

	_reg_write(reg_base + REG_SCL_DISP_GOP_LUT0,
		   (index << 16) | data);
	_reg_write(reg_base + REG_SCL_DISP_GOP_LUT1, (u32)~BIT(16));
	_reg_write(reg_base + REG_SCL_DISP_GOP_LUT1, BIT(16));
	return 0;
}

/**
 * sclr_gop_ow_set_cfg - set gop's osd-window configurations.
 *
 * @param inst: (0~4), the instance of gop which want to be configured.
 *		0~3 is on scl, 4 is on disp.
 * @param ow_inst: (0~7), the instance of ow which want to be configured.
 * @param cfg: ow's settings.
 */
void sclr_gop_ow_set_cfg(u8 inst, u8 ow_inst, struct sclr_gop_ow_cfg *cfg)
{
	static u8 reg_map_fmt[SCL_GOP_FMT_MAX] = {0, 0x4, 0x5, 0x8, 0xc};

	_reg_write(reg_base + REG_SCL_DISP_GOP_FMT(ow_inst),
		   reg_map_fmt[cfg->fmt]);
	_reg_write(reg_base + REG_SCL_DISP_GOP_H_RANGE(ow_inst),
		   (cfg->end.x << 16) | cfg->start.x);
	_reg_write(reg_base + REG_SCL_DISP_GOP_V_RANGE(ow_inst),
		   (cfg->end.y << 16) | cfg->start.y);
	_reg_write(reg_base + REG_SCL_DISP_GOP_ADDR_L(ow_inst),
		   cfg->addr);
	_reg_write(reg_base + REG_SCL_DISP_GOP_ADDR_H(ow_inst),
		   cfg->addr >> 32);
	_reg_write(reg_base + REG_SCL_DISP_GOP_PITCH(ow_inst),
		   cfg->pitch);
	_reg_write(reg_base + REG_SCL_DISP_GOP_SIZE(ow_inst),
		   (cfg->mem_size.h << 16) | cfg->mem_size.w);

	g_disp_cfg.gop_cfg.ow_cfg[ow_inst] = *cfg;
}

/****************************************************************************
 * SCALER DISP
 ****************************************************************************/
/**
 * sclr_disp_reg_shadow_sel - control the read reg-bank.
 *
 * @param read_shadow: true(shadow); false(working)
 */
void sclr_disp_reg_shadow_sel(bool read_shadow)
{
	_reg_write_mask(reg_base + REG_SCL_DISP_CFG, BIT(18),
			(read_shadow ? 0x0 : BIT(18)));
}

/**
 * sclr_disp_reg_shadow_mask - reg won't be update by sw/hw until unmask.
 *
 * @param mask: true(mask); false(unmask)
 * @return: mask status before thsi modification.
 */
bool sclr_disp_reg_shadow_mask(bool mask)
{
	bool is_masked = (_reg_read(reg_base + REG_SCL_DISP_CFG) & BIT(17));

	if (is_masked != mask)
		_reg_write_mask(reg_base + REG_SCL_DISP_CFG, BIT(17),
				(mask ? 0x0 : BIT(17)));

	return is_masked;
}

/**
 * sclr_disp_reg_force_up - trigger reg update by sw.
 *
 */
void sclr_disp_reg_force_up(void)
{
	_reg_write_mask(reg_base + REG_SCL_DISP_CFG, BIT(16), BIT(16));
}

/**
 * sclr_disp_tgen - enable timing-generator on disp.
 *
 * @param enable: AKA.
 * @return: tgen's enable status before change.
 */
bool sclr_disp_tgen_enable(bool enable)
{
	bool is_enable = (_reg_read(reg_base + REG_SCL_DISP_CFG) & 0x80);

	if (is_enable != enable) {
		_reg_write_mask(reg_base + REG_SCL_DISP_CFG, 0x0080,
				enable ? 0x80 : 0x00);
		g_disp_cfg.tgen_en = enable;
	}

	return is_enable;
}

/**
 * sclr_disp_set_cfg - set disp's configurations.
 *
 * @param cfg: disp's settings.
 */
void sclr_disp_set_cfg(struct sclr_disp_cfg *cfg)
{
	u32 tmp = 0;
	bool is_enable = false;

	tmp |= cfg->disp_from_sc;
	tmp |= (cfg->fmt << 12);
	if (cfg->sync_ext)
		tmp |= BIT(4);
	if (cfg->tgen_en)
		tmp |= BIT(7);
	if (cfg->dw1_en)
		tmp |= BIT(8);
	if (cfg->dw2_en)
		tmp |= BIT(9);

	is_enable = sclr_disp_reg_shadow_mask(false);
	if (!cfg->disp_from_sc) {
		sclr_disp_set_mem(&cfg->mem);
		_reg_write_mask(reg_base + REG_SCL_DISP_PITCH_Y, 0xf0000000,
				cfg->burst << 28);

		sclr_disp_set_in_csc(cfg->in_csc);
	}

	sclr_disp_set_out_csc(cfg->out_csc);

	_reg_write_mask(reg_base + REG_SCL_DISP_CFG, 0x0000ff9f, tmp);
	_reg_write_mask(reg_base + REG_SCL_DISP_CACHE, BIT(0), cfg->cache_mode);

	switch (cfg->out_bit) {
	case 6:
		tmp = 3 << 16;
		break;
	case 8:
		tmp = 2 << 16;
		break;
	default:
		tmp = 0;
		break;
	}
	tmp |= cfg->drop_mode << 18;
	_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR4, 0x000f0000, tmp);

	if (is_enable)
		sclr_disp_reg_shadow_mask(true);

	g_disp_cfg = *cfg;
}

/**
 * sclr_disp_get_cfg - get scl_disp's cfg
 *
 * @return: scl_disp's cfg
 */
struct sclr_disp_cfg *sclr_disp_get_cfg(void)
{
	return &g_disp_cfg;
}

/**
 * sclr_disp_set_timing - modify disp's timing-generator.
 *
 * @param timing: new timing of disp.
 */
void sclr_disp_set_timing(struct sclr_disp_timing *timing)
{
	u32 tmp = 0;
	bool is_enable = sclr_disp_tgen_enable(false);

	if (timing->vsync_pol)
		tmp |= 0x20;
	if (timing->hsync_pol)
		tmp |= 0x40;

	_reg_write_mask(reg_base + REG_SCL_DISP_CFG, 0x0060, tmp);
	_reg_write(reg_base + REG_SCL_DISP_TOTAL,
		   (timing->htotal << 16) | timing->vtotal);
	_reg_write(reg_base + REG_SCL_DISP_VSYNC,
		   (timing->vsync_end << 16) | timing->vsync_start);
	_reg_write(reg_base + REG_SCL_DISP_VFDE,
		   (timing->vfde_end << 16) | timing->vfde_start);
	_reg_write(reg_base + REG_SCL_DISP_VMDE,
		   (timing->vmde_end << 16) | timing->vmde_start);
	_reg_write(reg_base + REG_SCL_DISP_HSYNC,
		   (timing->hsync_end << 16) | timing->hsync_start);
	_reg_write(reg_base + REG_SCL_DISP_HFDE,
		   (timing->hfde_end << 16) | timing->hfde_start);
	_reg_write(reg_base + REG_SCL_DISP_HMDE,
		   (timing->hmde_end << 16) | timing->hmde_start);

	if (is_enable)
		sclr_disp_tgen_enable(true);
	disp_timing = *timing;
}

struct sclr_disp_timing *sclr_disp_get_timing(void)
{
	return &disp_timing;
}

/**
 * sclr_disp_set_rect - setup rect(me) of disp
 *
 * @param rect: the pos/size of me, which should fit with disp's input.
 */
int sclr_disp_set_rect(struct sclr_rect rect)
{
	bool is_enable = sclr_disp_reg_shadow_mask(false);

	if ((rect.y > disp_timing.vfde_end) ||
	    (rect.x > disp_timing.hfde_end) ||
	    ((disp_timing.vfde_start + rect.y + rect.h - 1) >
	      disp_timing.vfde_end) ||
	    ((disp_timing.hfde_start + rect.x + rect.w - 1) >
	      disp_timing.hfde_end)) {
		debug("[cvi-vip][sc] %s: me's pos(%d, %d) size(%d, %d) ",
		      __func__, rect.x, rect.y, rect.w, rect.h);
		debug(" out of range(%d, %d).\n",
		      disp_timing.hfde_end, disp_timing.vfde_end);
		return -EINVAL;
	}

	disp_timing.vmde_start = rect.y + disp_timing.vfde_start;
	disp_timing.hmde_start = rect.x + disp_timing.hfde_start;
	disp_timing.vmde_end = disp_timing.vmde_start + rect.h - 1;
	disp_timing.hmde_end = disp_timing.hmde_start + rect.w - 1;

	_reg_write(reg_base + REG_SCL_DISP_HMDE,
		   (disp_timing.hmde_end << 16) | disp_timing.hmde_start);
	_reg_write(reg_base + REG_SCL_DISP_VMDE,
		   (disp_timing.vmde_end << 16) | disp_timing.vmde_start);

	if (is_enable)
		sclr_disp_reg_shadow_mask(true);

	return 0;
}

/**
 * sclr_disp_set_mem - setup disp's mem settings. Only work if disp from mem.
 *
 * @param mem: mem settings for disp
 */
void sclr_disp_set_mem(struct sclr_mem *mem)
{
	bool is_enable = sclr_disp_reg_shadow_mask(false);

	_reg_write(reg_base + REG_SCL_DISP_OFFSET,
		   (mem->start_y << 16) | mem->start_x);
	_reg_write(reg_base + REG_SCL_DISP_SIZE,
		   ((mem->height - 1) << 16) | (mem->width - 1));
	_reg_write_mask(reg_base + REG_SCL_DISP_PITCH_Y, 0x00ffffff,
			mem->pitch_y);
	_reg_write(reg_base + REG_SCL_DISP_PITCH_C, mem->pitch_c);

	sclr_disp_set_addr(mem->addr0, mem->addr1, mem->addr2);
	if (is_enable)
		sclr_disp_reg_shadow_mask(true);

	g_disp_cfg.mem = *mem;
}

/**
 * sclr_disp_set_addr - setup disp's mem address. Only work if disp from mem.
 *
 * @param addr0: address of planar0
 * @param addr1: address of planar1
 * @param addr2: address of planar2
 */
void sclr_disp_set_addr(u64 addr0, u64 addr1, u64 addr2)
{
	_reg_write(reg_base + REG_SCL_DISP_ADDR0_L, addr0);
	_reg_write(reg_base + REG_SCL_DISP_ADDR0_H, addr0 >> 32);
	_reg_write(reg_base + REG_SCL_DISP_ADDR1_L, addr1);
	_reg_write(reg_base + REG_SCL_DISP_ADDR1_H, addr1 >> 32);
	_reg_write(reg_base + REG_SCL_DISP_ADDR2_L, addr2);
	_reg_write(reg_base + REG_SCL_DISP_ADDR2_H, addr2 >> 32);

	g_disp_cfg.mem.addr0 = addr0;
	g_disp_cfg.mem.addr1 = addr1;
	g_disp_cfg.mem.addr2 = addr2;
}

/**
 * sclr_disp_set_csc - configure disp's input CSC's coefficient/offset
 *
 * @param cfg: The settings for CSC
 */
void sclr_disp_set_csc(struct sclr_csc_matrix *cfg)
{
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC0, BIT(31) |
		   (cfg->coef[0][1] << 16) | (cfg->coef[0][0]));
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC1,
		   (cfg->coef[1][0] << 16) | (cfg->coef[0][2]));
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC2,
		   (cfg->coef[1][2] << 16) | (cfg->coef[1][1]));
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC3,
		   (cfg->coef[2][1] << 16) | (cfg->coef[2][0]));
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC4, (cfg->coef[2][2]));
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC_SUB,
		   (cfg->sub[2] << 16) | (cfg->sub[1] << 8) |
		   cfg->sub[0]);
	_reg_write(reg_base + REG_SCL_DISP_IN_CSC_ADD,
		   (cfg->add[2] << 16) | (cfg->add[1] << 8) |
		   cfg->add[0]);
}

/**
 * sclr_disp_set_in_csc - setup disp's csc on input. Only work if disp from mem.
 *
 * @param csc: csc settings
 */
void sclr_disp_set_in_csc(enum sclr_csc csc)
{
	if (csc == SCL_CSC_NONE)
		_reg_write(reg_base + REG_SCL_DISP_IN_CSC0, 0);
	else if (csc < SCL_CSC_MAX)
		sclr_disp_set_csc(&csc_mtrx[csc]);

	g_disp_cfg.in_csc = csc;
}

/**
 * sclr_disp_set_out_csc - setup disp's csc on output.
 *
 * @param csc: csc settings
 */
void sclr_disp_set_out_csc(enum sclr_csc csc)
{
	if (csc == SCL_CSC_NONE) {
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC0, 0);
	} else if (csc < SCL_CSC_MAX) {
		struct sclr_csc_matrix *cfg = &csc_mtrx[csc];

		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC0, BIT(31) |
			   (cfg->coef[0][1] << 16) | (cfg->coef[0][0]));
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC1,
			   (cfg->coef[1][0] << 16) | (cfg->coef[0][2]));
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC2,
			   (cfg->coef[1][2] << 16) | (cfg->coef[1][1]));
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC3,
			   (cfg->coef[2][1] << 16) | (cfg->coef[2][0]));
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC4, (cfg->coef[2][2]));
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC_SUB,
			   (cfg->sub[2] << 16) | (cfg->sub[1] << 8) |
			   cfg->sub[0]);
		_reg_write(reg_base + REG_SCL_DISP_OUT_CSC_ADD,
			   (cfg->add[2] << 16) | (cfg->add[1] << 8) |
			   cfg->add[0]);
	}

	g_disp_cfg.out_csc = csc;
}

/**
 * sclr_disp_set_pattern - setup disp's pattern generator.
 *
 * @param type: type of pattern
 * @param color: color of pattern. Only for Gradient/FULL type.
 */
void sclr_disp_set_pattern(enum sclr_disp_pat_type type,
			   enum sclr_disp_pat_color color, const u16 *rgb)
{
	switch (type) {
	case SCL_PAT_TYPE_OFF:
		_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0x16, 0);
		break;

	case SCL_PAT_TYPE_SNOW:
		_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0x16, 0x10);
		break;

	case SCL_PAT_TYPE_AUTO:
		_reg_write(reg_base + REG_SCL_DISP_PAT_COLOR0, 0x03ff03ff);
		_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR1, 0x000003ff, 0x3ff);
		_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0xff0016,
				0x780006);
		break;

	case SCL_PAT_TYPE_V_GRAD:
	case SCL_PAT_TYPE_H_GRAD:
	case SCL_PAT_TYPE_FULL: {
		if (color == SCL_PAT_COLOR_USR) {
			_reg_write(reg_base + REG_SCL_DISP_PAT_COLOR0, rgb[1] << 16 | rgb[0]);
			_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR1, 0x000003ff, rgb[2]);
			_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0x1f000016,
					(type << 27) | (SCL_PAT_COLOR_WHITE << 24) | 0x0002);
		} else {
			_reg_write(reg_base + REG_SCL_DISP_PAT_COLOR0, 0x03ff03ff);
			_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR1, 0x000003ff, 0x3ff);
			_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0x1f000016,
					(type << 27) | (color << 24) | 0x0002);
		}
		break;
	}
	default:
		printf("%s - unacceptiable pattern-type(%d)\n", __func__, type);
		break;
	}
}

/**
 * sclr_disp_set_frame_bgcolro - setup disp frame(area outside mde)'s
 *				 background color.
 *
 * @param r: 10bit red value
 * @param g: 10bit green value
 * @param b: 10bit blue value
 */
void sclr_disp_set_frame_bgcolor(u16 r, u16 g, u16 b)
{
	_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR1, 0x0fff0000,
			r << 16);
	_reg_write(reg_base + REG_SCL_DISP_PAT_COLOR2, b << 16 | g);
}

/**
 * sclr_disp_set_window_bgcolro - setup disp window's background color.
 *
 * @param r: 10bit red value
 * @param g: 10bit green value
 * @param b: 10bit blue value
 */
void sclr_disp_set_window_bgcolor(u16 r, u16 g, u16 b)
{
	_reg_write(reg_base + REG_SCL_DISP_PAT_COLOR3, g << 16 | r);
	_reg_write_mask(reg_base + REG_SCL_DISP_PAT_COLOR4, 0x0fff, b);
}

/**
 * sclr_disp_enable_window_bgcolor - Use window bg-color to hide everything
 *				     including test-pattern.
 *
 * @param enable: enable window bgcolor or not.
 */
void sclr_disp_enable_window_bgcolor(bool enable)
{
	_reg_write_mask(reg_base + REG_SCL_DISP_PAT_CFG, 0x20, enable ? 0x20 : 0);
}

union sclr_disp_dbg_status sclr_disp_get_dbg_status(bool clr)
{
	union sclr_disp_dbg_status status;

	status.raw = _reg_read(reg_base + REG_SCL_DISP_DBG);

	if (clr) {
		status.b.err_fwr_clr = 1;
		status.b.err_erd_clr = 1;
		status.b.bw_fail_clr = 1;
		_reg_write(reg_base + REG_SCL_DISP_DBG, status.raw);
	}

	return status;
}

void sclr_disp_gamma_ctrl(bool enable, bool pre_osd)
{
	u32 value = 0;

	if (enable)
		value |= 0x04;
	if (pre_osd)
		value |= 0x08;
	_reg_write_mask(reg_base + REG_SCL_DISP_GAMMA_CTRL, 0x0C, value);
}

void sclr_disp_gamma_lut_update(const u8 *b, const u8 *g, const u8 *r)
{
	u8 i;
	u32 value;

	_reg_write_mask(reg_base + REG_SCL_DISP_GAMMA_CTRL, 0x03, 0x03);

	for (i = 0; i < 65; ++i) {
		value = *(b + i) | (*(g + i) << 8) | (*(r + i) << 16)
			| (i << 24) | 0x80000000;
		_reg_write(reg_base + REG_SCL_DISP_GAMMA_WR_LUT, value);
	}

	_reg_write_mask(reg_base + REG_SCL_DISP_GAMMA_CTRL, 0x03, 0x00);
}

void sclr_lvdstx_set(union sclr_lvdstx cfg)
{
	_reg_write(reg_base + REG_SCL_TOP_LVDSTX, cfg.raw);
}

void sclr_lvdstx_get(union sclr_lvdstx *cfg)
{
	cfg->raw = _reg_read(reg_base + REG_SCL_TOP_LVDSTX);
}

void sclr_bt_set(union sclr_bt_enc enc, union sclr_bt_sync_code sync)
{
	_reg_write(reg_base + REG_SCL_TOP_BT_ENC, enc.raw);
	_reg_write(reg_base + REG_SCL_TOP_BT_SYNC_CODE, sync.raw);
}

void sclr_bt_get(union sclr_bt_enc *enc, union sclr_bt_sync_code *sync)
{
	enc->raw = _reg_read(reg_base + REG_SCL_TOP_BT_ENC);
	sync->raw = _reg_read(reg_base + REG_SCL_TOP_BT_SYNC_CODE);
}

void sclr_disp_mux_sel(enum sclr_vo_sel sel)
{
	_reg_write_mask(reg_base + REG_SCL_TOP_VO_MUX, 0x07, sel);
}

void sclr_disp_set_intf(enum sclr_vo_intf intf)
{
	bool data_en[5] = {true, true, true, true};

	if (intf == SCLR_VO_INTF_DISABLE)
		_reg_write_mask(0x03002840, 0xa, 0);
	else
		_reg_write_mask(0x03002840, 0xa, 0xa);

	dphy_init(intf);

	if (intf == SCLR_VO_INTF_DISABLE) {
		sclr_disp_mux_sel(SCLR_VO_SEL_DISABLE);
	} else if ((intf == SCLR_VO_INTF_BT601) || (intf == SCLR_VO_INTF_BT656) || (intf == SCLR_VO_INTF_BT1120)) {
		if (intf == SCLR_VO_INTF_BT601)
			sclr_disp_mux_sel(SCLR_VO_SEL_BT601);
		else if (intf == SCLR_VO_INTF_BT656)
			sclr_disp_mux_sel(SCLR_VO_SEL_BT656);
		else if (intf == SCLR_VO_INTF_BT1120)
			sclr_disp_mux_sel(SCLR_VO_SEL_BT1120);
		dphy_dsi_lane_en(true, data_en, false);
	} else if (intf == SCLR_VO_INTF_I80) {
		sclr_disp_mux_sel(SCLR_VO_SEL_I80);
		dphy_dsi_lane_en(true, data_en, false);
		_reg_write_mask(reg_base + REG_SCL_DISP_MCU_IF_CTRL, BIT(0), 1);
	} else if (intf == SCLR_VO_INTF_SW) {
		sclr_disp_mux_sel(SCLR_VO_SEL_SW);
	} else if (intf == SCLR_VO_INTF_MIPI) {
		sclr_disp_mux_sel(SCLR_VO_SEL_DISABLE);
	} else if (intf == SCLR_VO_INTF_LVDS) {
		sclr_disp_mux_sel(SCLR_VO_SEL_DISABLE);
	}
}

/**
 * sclr_dsi_get_mode - get current dsi mode
 *
 * @return: current dsi mode
 */
enum sclr_dsi_mode sclr_dsi_get_mode(void)
{
	return (_reg_read(reg_base + REG_SCL_DSI_MAC_EN) & 0x0f);
}

/**
 * sclr_dsi_clr_mode - let dsi back to idle mode
 *
 */
void sclr_dsi_clr_mode(void)
{
	u32 mode = _reg_read(reg_base + REG_SCL_DSI_MAC_EN);

	if (mode != SCLR_DSI_MODE_IDLE)
		_reg_write(reg_base + REG_SCL_DSI_MAC_EN, mode);
}

/**
 * sclr_dsi_set_mode - set dsi mode
 *
 * @param mode: new dsi mode except for idle
 * @return: 0 if success
 */
int sclr_dsi_set_mode(enum sclr_dsi_mode mode)
{
	if (mode >= SCLR_DSI_MODE_MAX)
		return -1;

	if (mode == SCLR_DSI_MODE_IDLE) {
		sclr_dsi_clr_mode();
		return 0;
	}

	if (_reg_read(reg_base + REG_SCL_DSI_MAC_EN))
		return -1;

	_reg_write(reg_base + REG_SCL_DSI_MAC_EN, mode);
	return 0;
}

/**
 * sclr_dsi_chk_mode_done - check if dsi's work done.
 *
 * @param mode: the mode to check.
 * @return: 0 if success
 */
int sclr_dsi_chk_mode_done(enum sclr_dsi_mode mode)
{
	u32 val = 0;

	if ((mode == SCLR_DSI_MODE_ESC) || (mode == SCLR_DSI_MODE_SPKT)) {
		val = _reg_read(reg_base + REG_SCL_DSI_MAC_EN) & 0xf0;
		return (val ^ (mode << 4)) ? -1 : 0;
	}

	if ((mode == SCLR_DSI_MODE_IDLE) || (mode == SCLR_DSI_MODE_HS)) {
		val = _reg_read(reg_base + REG_SCL_DSI_MAC_EN) & 0x0f;
		return (val == (mode)) ? 0 : -1;
	}

	return -1;
}

int _dsi_chk_and_clean_mode(enum sclr_dsi_mode mode)
{
	int i, ret;

	for (i = 0; i < 5; ++i) {
		udelay(20);
		ret = sclr_dsi_chk_mode_done(mode);
		if (ret == 0) {
			sclr_dsi_clr_mode();
			break;
		}
	}
	return ret;
}

#define POLY 0x8408
/*
 *                                      16   12   5
 * this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
 * This works out to be 0x1021, but the way the algorithm works
 * lets us use 0x8408 (the reverse of the bit pattern).  The high
 * bit is always assumed to be set, thus we only use 16 bits to
 * represent the 17 bit value.
 */
static u16 crc16(unsigned char *data_p, unsigned short length)
{
	u8 i, data;
	u16 crc = 0xffff;

	if (length == 0)
		return (~crc);

	do {
		for (i = 0, data = 0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
		}
	} while (--length);

	return crc;
}

static unsigned char ecc(unsigned char *data)
{
	char D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12;
	char D13, D14, D15, D16, D17, D18, D19, D20, D21, D22, D23;
	char P0, P1, P2, P3, P4, P5, P6, P7;

	D0  = data[0] & 0x01;
	D1  = (data[0] >> 1) & 0x01;
	D2  = (data[0] >> 2) & 0x01;
	D3  = (data[0] >> 3) & 0x01;
	D4  = (data[0] >> 4) & 0x01;
	D5  = (data[0] >> 5) & 0x01;
	D6  = (data[0] >> 6) & 0x01;
	D7  = (data[0] >> 7) & 0x01;

	D8  = data[1] & 0x01;
	D9  = (data[1] >> 1) & 0x01;
	D10 = (data[1] >> 2) & 0x01;
	D11 = (data[1] >> 3) & 0x01;
	D12 = (data[1] >> 4) & 0x01;
	D13 = (data[1] >> 5) & 0x01;
	D14 = (data[1] >> 6) & 0x01;
	D15 = (data[1] >> 7) & 0x01;

	D16 = data[2] & 0x01;
	D17 = (data[2] >> 1) & 0x01;
	D18 = (data[2] >> 2) & 0x01;
	D19 = (data[2] >> 3) & 0x01;
	D20 = (data[2] >> 4) & 0x01;
	D21 = (data[2] >> 5) & 0x01;
	D22 = (data[2] >> 6) & 0x01;
	D23 = (data[2] >> 7) & 0x01;

	P7 = 0;
	P6 = 0;
	P5 = (D10 ^ D11 ^ D12 ^ D13 ^ D14 ^ D15 ^ D16 ^ D17 ^ D18 ^ D19 ^ D21 ^ D22 ^ D23) & 0x01;
	P4 = (D4 ^ D5 ^ D6 ^ D7 ^ D8 ^ D9 ^ D16 ^ D17 ^ D18 ^ D19 ^ D20 ^ D22 ^ D23) & 0x01;
	P3 = (D1 ^ D2 ^ D3 ^ D7 ^ D8 ^ D9 ^ D13 ^ D14 ^ D15 ^ D19 ^ D20 ^ D21 ^ D23) & 0x01;
	P2 = (D0 ^ D2 ^ D3 ^ D5 ^ D6 ^ D9 ^ D11 ^ D12 ^ D15 ^ D18 ^ D20 ^ D21 ^ D22) & 0x01;
	P1 = (D0 ^ D1 ^ D3 ^ D4 ^ D6 ^ D8 ^ D10 ^ D12 ^ D14 ^ D17 ^ D20 ^ D21 ^ D22 ^ D23) & 0x01;
	P0 = (D0 ^ D1 ^ D2 ^ D4 ^ D5 ^ D7 ^ D10 ^ D11 ^ D13 ^ D16 ^ D20 ^ D21 ^ D22 ^ D23) & 0x01;

	return (P7 << 7) | (P6 << 6) | (P5 << 5) | (P4 << 4) |
		(P3 << 3) | (P2 << 2) | (P1 << 1) | P0;
}

/**
 * sclr_dsi_long_packet_raw - send dsi long packet by escapet-lpdt.
 *
 * @param data: long packet data including header and crc.
 * @param count: number of long packet data, 16 at most.
 * @return: 0 if success
 */
int sclr_dsi_long_packet_raw(const u8 *data, u8 count)
{
	u64 addr = reg_base + REG_SCL_DSI_ESC_TX0;
	u32 val = 0;
	u8 i = 0;

	if ((count > SCL_MAX_DSI_LP) || (count == 0)) {
		printf("%s: count(%d) invalid\n", __func__, count);
		return -1;
	}

	val = 0x01 | ((count - 1) << 8);
	_reg_write(reg_base + REG_SCL_DSI_ESC, val);
	for (i = 0; i < count; i += 4) {
		if (count - i < 4) {
			val = 0;
			memcpy(&val, &data[i], count - i);
			_reg_write(addr + i, val);
			break;
		}
		memcpy(&val, &data[i], 4);
		_reg_write(addr + i, val);
	}

	sclr_dsi_set_mode(SCLR_DSI_MODE_ESC);
	return _dsi_chk_and_clean_mode(SCLR_DSI_MODE_ESC);
}

/**
 * sclr_dsi_long_packet - send dsi long packet by escapet-lpdt.
 *
 * @param di: data ID
 * @param data: long packet data
 * @param count: number of long packet data, 100 at most.
 * @param sw_mode: use sw-overwrite to create dcs cmd
 * @return: 0 if success
 */
int sclr_dsi_long_packet(u8 di, const u8 *data, u8 count, bool sw_mode)
{
	u8 packet[100] = {di, count, 0, 0};
	u16 crc;

	packet[3] = ecc(packet);
	memcpy(&packet[4], data, count);
	count += 4;
	crc = crc16(packet, count);
	packet[count++] = crc & 0xff;
	packet[count++] = crc >> 8;

	if (!sw_mode && count < SCL_MAX_DSI_LP)
		return sclr_dsi_long_packet_raw(packet, count);

	dpyh_mipi_tx_manual_packet(packet, count);
	return 0;
}

/**
 * sclr_dsi_short_packet - send dsi short packet by escapet-lpdt.
 *   *NOTE*: ecc is hw-generated.
 *
 * @param di: data ID
 * @param data: short packet data
 * @param count: number of short packet data, 1 or 2.
 * @param sw_mode: use sw-overwrite to create dcs cmd
 * @return: 0 if success
 */
int sclr_dsi_short_packet(u8 di, const u8 *data, u8 count, bool sw_mode)
{
	u32 val = 0;

	if ((count > SCL_MAX_DSI_SP) || (count == 0))
		return -1;

	val = di;
	if (count == 2) {
		//val = 0x15;
		val |= (data[0] << 8) | (data[1] << 16);
	} else {
		//val = 0x05;
		val |= data[0] << 8;
	}

	if (!sw_mode) {
		_reg_write_mask(reg_base + REG_SCL_DSI_HS_0, 0x00ffffff, val);
		sclr_dsi_set_mode(SCLR_DSI_MODE_SPKT);
		return _dsi_chk_and_clean_mode(SCLR_DSI_MODE_SPKT);
	}

	val |= (ecc((u8 *)&val) << 24);
	dpyh_mipi_tx_manual_packet((u8 *)&val, 4);
	return 0;
}

/**
 * sclr_dsi_dcs_write_buffer - send dsi packet by escapet-lpdt.
 *
 * @param di: data ID
 * @param data: packet data
 * @param count: number of packet data
 * @param sw_mode: use sw-overwrite to create dcs cmd
 * @return: Zero on success or a negative error code on failure.
 */
int sclr_dsi_dcs_write_buffer(u8 di, const void *data, size_t len, bool sw_mode)
{
	if (len == 0) {
		printf("[cvi_mipi_tx] %s: 0 param unacceptable.\n", __func__);
		return -1;
	}

	if ((di == 0x06) || (di == 0x05) || (di == 0x04) || (di == 0x03)) {
		if (len != 1) {
			printf("[cvi_mipi_tx] %s: cmd(0x%02x) should has 1 param.\n", __func__, di);
			return -1;
		}
		return sclr_dsi_short_packet(di, data, len, sw_mode);
	}
	if ((di == 0x15) || (di == 0x37) || (di == 0x13) || (di == 0x14)) {
		if (len != 2) {
			printf("[cvi_mipi_tx] %s: cmd(0x%02x) should has 2 param.\n", __func__, di);
			return -1;
		}
		return sclr_dsi_short_packet(di, data, len, sw_mode);
	}
	if ((di == 0x29) || (di == 0x39))
		return sclr_dsi_long_packet(di, data, len, sw_mode);

	return sclr_dsi_long_packet(di, data, len, sw_mode);
}

#define ACK_WR       0x02
#define GEN_READ_LP  0x1A
#define GEN_READ_SP1 0x11
#define GEN_READ_SP2 0x12
#define DCS_READ_LP  0x1C
#define DCS_READ_SP1 0x21
#define DCS_READ_SP2 0x22

int sclr_dsi_dcs_read_buffer(u8 di, const u16 data_param, u8 *data, size_t len, bool sw_mode)
{
	int ret = 0;
	u32 rx_data;
	int i = 0;

	if (len > 4)
		len = 4;

	if (sclr_dsi_get_mode() == SCLR_DSI_MODE_HS) {
		printf("[cvi_mipi_tx] %s: not work in HS.\n", __func__);
		return -1;
	}

	// only set necessery bits
	_reg_write_mask(reg_base + REG_SCL_DSI_ESC, 0x07, 0x04);

	// send read cmd
	sclr_dsi_short_packet(di, (u8 *)&data_param, 2, sw_mode);

	// goto BTA
	sclr_dsi_set_mode(SCLR_DSI_MODE_ESC);
	udelay(20);
	ret = sclr_dsi_chk_mode_done(SCLR_DSI_MODE_ESC);
	if (ret == 0) {
		sclr_dsi_clr_mode();
	} else {
		printf("[cvi_mipi_tx] %s: BTA error.\n", __func__);
		return ret;
	}

	// check result
	rx_data = _reg_read(reg_base + REG_SCL_DSI_ESC_RX0);
	switch (rx_data & 0xff) {
	case GEN_READ_SP1:
	case DCS_READ_SP1:
		data[0] = (rx_data >> 8) & 0xff;
		break;
	case GEN_READ_SP2:
	case DCS_READ_SP2:
		data[0] = (rx_data >> 8) & 0xff;
		data[1] = (rx_data >> 16) & 0xff;
		break;
	case GEN_READ_LP:
	case DCS_READ_LP:
		rx_data = _reg_read(reg_base + REG_SCL_DSI_ESC_RX1);
		for (i = 0; i < len; ++i)
			data[i] = (rx_data >> (i * 8)) & 0xff;
		break;
	case ACK_WR:
		printf("[cvi_mipi_tx] %s: dcs read, ack with error(%#x %#x).\n"
			, __func__, (rx_data >> 8) & 0xff, (rx_data >> 16) & 0xff);
		ret = -1;
		break;
	default:
		printf("[cvi_mipi_tx] %s: unknown DT, %#x.", __func__, rx_data);
		ret = -1;
		break;
	}

	//debug("%s: %#x %#x\n", __func__, rx_data0, rx_data1);
	return ret;
}

int sclr_dsi_config(u8 lane_num, enum sclr_dsi_fmt fmt, u16 width)
{
	u32 val = 0;
	u8 bit_depth[] = {24, 18, 16, 30};

	if ((lane_num != 1) && (lane_num != 2) && (lane_num != 4))
		return -EINVAL;
	if (fmt > SCLR_DSI_FMT_MAX)
		return -EINVAL;

	lane_num >>= 1;
	val = (fmt << 30) | (lane_num << 24);
	_reg_write_mask(reg_base + REG_SCL_DSI_HS_0, 0xc3000000, val);
	val = (width / 10) << 16 | ((width * bit_depth[fmt] + 7) >> 3);
	_reg_write(reg_base + REG_SCL_DSI_HS_1, val);

	return 0;
}

void sclr_i80_sw_mode(bool enable)
{
	_reg_write_mask(reg_base + REG_SCL_DISP_MCU_IF_CTRL, BIT(11) | BIT(1), enable ? 0x802 : 0x000);

	if (enable) {
		sclr_disp_tgen_enable(true);
		mdelay(40);
		sclr_disp_tgen_enable(false);
	}
}

void sclr_i80_packet(u32 cmd)
{
	u8 cnt = 0;

	_reg_write(reg_base + REG_SCL_DISP_MCU_SW_CTRL, cmd);
	_reg_write_mask(reg_base + REG_SCL_DISP_MCU_SW_CTRL, BIT(31), BIT(31));

	do {
		udelay(1);
		if (_reg_read(reg_base + REG_SCL_DISP_MCU_SW_CTRL) & BIT(24))
			break;
	} while (++cnt < 10);

	if (cnt == 10)
		printf("[cvi_vip] %s: cmd(%#x) not ready.\n", __func__, cmd);
}

void sclr_i80_run(void)
{
	u8 cnt = 0;

	_reg_write_mask(reg_base + REG_SCL_DISP_MCU_IF_CTRL, BIT(11), BIT(11));

	do {
		udelay(5 * 1000);
		if (_reg_read(reg_base + REG_SCL_DISP_MCU_STATUS) == 0x08)
			break;
	} while (++cnt < 10);

	if (cnt == 10) {
		printf("[cvi_vip] %s: not finish. sw clear it.\n", __func__);
		_reg_write_mask(reg_base + REG_SCL_DISP_MCU_IF_CTRL, BIT(10), BIT(10));
	}
}

/****************************************************************************
 * SCALER CTRL
 ****************************************************************************/
/**
 * sclr_ctrl_init - setup all sc instances.
 *
 */
void sclr_ctrl_init(void)
{
	union sclr_intr intr_mask;
	bool disp_from_sc = false;

	// init variables
	memset(&g_top_cfg, 0, sizeof(g_top_cfg));
	memset(&g_disp_cfg, 0, sizeof(g_disp_cfg));
	memset(&disp_timing, 0, sizeof(disp_timing));

	g_top_cfg.ip_trig_src = true;
	g_top_cfg.sclr_enable[0] = false;
	g_top_cfg.sclr_enable[1] = false;
	g_top_cfg.sclr_enable[2] = false;
	g_top_cfg.sclr_enable[3] = false;
	g_top_cfg.disp_enable = false;
	g_top_cfg.disp_from_sc = disp_from_sc;
	g_top_cfg.img_in_d_trig_src = SCL_IMG_TRIG_SRC_SW;
	g_top_cfg.img_in_v_trig_src = SCL_IMG_TRIG_SRC_SW;
	g_disp_cfg.disp_from_sc = disp_from_sc;
	g_disp_cfg.cache_mode = true;
	g_disp_cfg.sync_ext = false;
	g_disp_cfg.tgen_en = false;
	g_disp_cfg.fmt = SCL_FMT_RGB_PLANAR;
	g_disp_cfg.in_csc = SCL_CSC_NONE;
	g_disp_cfg.out_csc = SCL_CSC_NONE;
	g_disp_cfg.burst = 7;
	g_disp_cfg.out_bit = 8;
	g_disp_cfg.drop_mode = SCL_DISP_DROP_MODE_DITHER;

	// init hw
	sclr_top_set_cfg(&g_top_cfg);

	sclr_disp_reg_shadow_sel(false);
	sclr_disp_tgen_enable(false);
	sclr_disp_set_cfg(&g_disp_cfg);

	intr_mask.b.img_in_d_frame_end = true;
	intr_mask.b.img_in_v_frame_end = true;
	intr_mask.b.scl0_frame_end = true;
	intr_mask.b.scl1_frame_end = true;
	intr_mask.b.scl2_frame_end = true;
	intr_mask.b.scl3_frame_end = true;
	intr_mask.b.prog_too_late = true;
	intr_mask.b.cmdq = true;
	intr_mask.b.disp_frame_end = true;
	sclr_set_intr_mask(intr_mask);

	sclr_top_reg_done();
	sclr_top_reg_force_up();
	sclr_top_pg_late_clr();
}

/**
 * sclr_ctrl_set_disp_src - setup input-src of disp.
 *
 * @param disp_from_sc: true(from sc_0); false(from mem)
 * @return: 0 if success
 */
int sclr_ctrl_set_disp_src(bool disp_from_sc)
{
	g_top_cfg.disp_from_sc = disp_from_sc;
	g_disp_cfg.disp_from_sc = disp_from_sc;

	sclr_top_set_cfg(&g_top_cfg);
	sclr_disp_set_cfg(&g_disp_cfg);

	return 0;
}

int set_disp_ctrl_gpios(struct disp_ctrl_gpios *ctrl_gpios)
{
	g_disp_cfg.ctrl_gpios = *ctrl_gpios;

	return 0;
}

int get_disp_ctrl_gpios(struct disp_ctrl_gpios *ctrl_gpios)
{
	*ctrl_gpios = g_disp_cfg.ctrl_gpios;

	return 0;
}

