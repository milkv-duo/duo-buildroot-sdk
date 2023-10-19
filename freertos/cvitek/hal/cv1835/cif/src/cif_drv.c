#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/unistd.h"
//#include <stdint.h>
#include "reg.h"
#include "cif_reg.h"
#include "cif_drv.h"

/* SubLVDS Normal Sync code */
#define SLVDS_SYNC_CODE_1ST		0xFFFu
#define SLVDS_SYNC_CODE_2ND		0u
#define SLVDS_SYNC_CODE_3RD		0u
#define SLVDS_SYNC_CODE_NORM_BK_SAV	0xAB0u
#define SLVDS_SYNC_CODE_NORM_BK_EAV	0xB60u
#define SLVDS_SYNC_CODE_NORM_SAV	0x800u
#define SLVDS_SYNC_CODE_NORM_EAV	0x9D0u
#define SLVDS_SYNC_CODE_N0_BK_SAV	0x2B0u
#define SLVDS_SYNC_CODE_N0_BK_EAV	0x360u
#define SLVDS_SYNC_CODE_N1_BK_SAV	0x6B0u
#define SLVDS_SYNC_CODE_N1_BK_EAV	0x760u

/* 10 bit SubLVDS HDR-realted Sync code */
#define SLVDS_SYNC_CODE_N0_LEF_SAV	0x004u
#define SLVDS_SYNC_CODE_N0_LEF_EAV	0x1D4u
#define SLVDS_SYNC_CODE_N0_SEF_SAV	0x008u
#define SLVDS_SYNC_CODE_N0_SEF_EAV	0x1D8u
#define SLVDS_SYNC_CODE_N1_LEF_SAV	0x404u
#define SLVDS_SYNC_CODE_N1_LEF_EAV	0x5D4u
#define SLVDS_SYNC_CODE_N1_SEF_SAV	0x408u
#define SLVDS_SYNC_CODE_N1_SEF_EAV	0x5D8u
#define SLVDS_SYNC_CODE_N0_LSEF_SAV	0x00Cu
#define SLVDS_SYNC_CODE_N0_LSEF_EAV	0x1DCu
#define SLVDS_SYNC_CODE_N1_LSEF_SAV	0x40Cu
#define SLVDS_SYNC_CODE_N1_LSEF_EAV	0x5DCu

/* HiSPi PKT-SP HDR-realted Sync code */
#define HISPI_SYNC_CODE_T1_SOL		0x800u
#define HISPI_SYNC_CODE_T1_EOL		0xA00u
#define HISPI_SYNC_CODE_T2_SOL		0x820u
#define HISPI_SYNC_CODE_T2_EOL		0xA20u
#define HISPI_SYNC_CODE_T1_SOF		0xC00u
#define HISPI_SYNC_CODE_T1_EOF		0xE00u
#define HISPI_SYNC_CODE_T2_SOF		0xC20u
#define HISPI_SYNC_CODE_T2_EOF		0xE20u
#define HISPI_SYNC_CODE_VSYNC_GEN	0xC00u

/****************************************************************************
 * Global parameters
 ****************************************************************************/
static uintptr_t mac_reg_base[MAX_LINK_NUM];
static uintptr_t wrap_reg_base[MAX_LINK_NUM];

static uint64_t m_cif_mac_phys_base_list[MAX_LINK_NUM][CIF_MAC_BLK_ID_MAX];
static uint64_t m_cif_wrap_phys_base_list[MAX_LINK_NUM][CIF_WRAP_BLK_ID_MAX];

/****************************************************************************
 * Interfaces
 ****************************************************************************/
void cif_set_base_addr(uint32_t link, void *mac_base, void *wrap_base)
{
	uint64_t *addr = cif_get_mac_phys_reg_bases(link);
	int i = 0;

	for (i = 0; i < CIF_MAC_BLK_ID_MAX; ++i) {
		addr[i] -= mac_reg_base[link];
		addr[i] += (uintptr_t)mac_base;
	}
	mac_reg_base[link] = (uintptr_t)mac_base;

	addr = cif_get_wrap_phys_reg_bases(link);

	for (i = 0; i < CIF_WRAP_BLK_ID_MAX; ++i) {
		addr[i] -= wrap_reg_base[link];
		addr[i] += (uintptr_t)wrap_base;
	}
	wrap_reg_base[link] = (uintptr_t)mac_base;
}

void cif_init(uint32_t link, struct cif_ctx *ctx)
{
	cif_set_base_addr(link, (link == 0) ?(void *)CIF_MAC_TOP_PHY_REG_BASE : (void *)CIF_MAC1_TOP_PHY_REG_BASE,
			(link == 0) ? (void *)CIF_WRAP_TOP_PHY_REG_BASE : (void *)CIF_WRAP1_TOP_PHY_REG_BASE);
	ctx->mac_phys_regs = m_cif_mac_phys_base_list[link];
	ctx->wrap_phys_regs = m_cif_wrap_phys_base_list[link];
}

void cif_uninit(void)
{
}

void cif_reset(void)
{
}

static void _cif_sublvds_config(struct cif_ctx *ctx,
			       struct param_sublvds *param)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];
	struct sync_code_s *sc = &param->sync_code;

	/* Config the sync code */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_18, SLVDS_N0_LEF_SAV,
			       sc->slvds.n0_lef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_1C, SLVDS_N0_LEF_EAV,
			       sc->slvds.n0_lef_eav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_1C, SLVDS_N0_SEF_SAV,
			       sc->slvds.n0_sef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_20, SLVDS_N0_SEF_EAV,
			       sc->slvds.n0_sef_eav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_20, SLVDS_N1_LEF_SAV,
			       sc->slvds.n1_lef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_24, SLVDS_N1_LEF_EAV,
			       sc->slvds.n1_lef_eav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_24, SLVDS_N1_SEF_SAV,
			       sc->slvds.n1_sef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_28, SLVDS_N1_SEF_EAV,
			       sc->slvds.n1_sef_eav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_50, SLVDS_N0_LSEF_SAV,
			       sc->slvds.n0_lsef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_50, SLVDS_N0_LSEF_EAV,
			       sc->slvds.n0_lsef_eav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_54, SLVDS_N1_LSEF_SAV,
			       sc->slvds.n1_lsef_sav);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_54, SLVDS_N1_LSEF_EAV,
			       sc->slvds.n1_lsef_eav);

	/* Config the sensor mode */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SENSOR_MAC_MODE,
			     1);
	/* invert the HS/VS/HDR */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_VS_INV,
			     1);
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_HS_INV,
			     1);
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_HDR_INV,
			     1);
	/* subLVDS controller enable */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_CTRL_ENABLE,
			     1);
	/* disable HiSPi mode */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_60, HISPI_MODE,
			       0);
	/* Config the lane enable */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_ENABLE,
			       (1 << param->lane_num) - 1);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_30, SLVDS_LANE_MODE,
			       param->lane_num - 1);
	/* Config the raw format. */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_BIT_MODE,
			       param->fmt);
	/* Config the endian. */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_DATA_REVERSE,
			       param->endian == CIF_SLVDS_ENDIAN_LSB);
	/* DPHY sensor mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SENSOR_MODE,
			      1);
	/* DPHY bit mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SLVDS_BIT_MODE,
			      param->fmt);
	/* DPHY endian mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SLVDS_INV_EN,
			      param->wrap_endian == CIF_SLVDS_ENDIAN_MSB);
}

static void _cif_hispi_config(struct cif_ctx *ctx,
			     struct param_hispi *param)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	/* Config the sensor mode */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SENSOR_MAC_MODE,
			     1);
	/* invert the HS/VS */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_VS_INV,
			     1);
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_HS_INV,
			     1);
	/* subLVDS controller enable */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SUBLVDS_CTRL_ENABLE,
			     1);
	/* Config the raw format. */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_BIT_MODE,
			       param->fmt);
	/* Enable HiSPi mode */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_60, HISPI_MODE,
			       1);
	/* Config the lane enable */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_ENABLE,
			       (1 << param->lane_num) - 1);
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_30, SLVDS_LANE_MODE,
			       param->lane_num - 1);
	/* Config the endian. */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_DATA_REVERSE,
			       param->endian);
	/* Config the HiSPi mode*/
	if (param->mode == CIF_HISPI_MODE_PKT_SP) {
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_60, HISPI_USE_HSIZE,
				       0);
	} else {
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_58, SLVDS_HDR_P2_HSIZE,
				       param->h_size/param->lane_num);
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_60, HISPI_USE_HSIZE,
				       1);
	}
	/* DPHY sensor mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SENSOR_MODE,
			      1);
	/* DPHY bit mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SLVDS_BIT_MODE,
			      param->fmt);
	/* DPHY endian mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SLVDS_INV_EN,
			      param->wrap_endian == CIF_SLVDS_ENDIAN_MSB);
}

static void _cif_ttl_config(struct cif_ctx *ctx,
			   struct param_ttl *param)
{
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];
	uint64_t wrap_top = m_cif_wrap_phys_base_list[1][CIF_WRAP_BLK_ID_TOP];

	/* Config the sensor mode */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SENSOR_MAC_MODE,
			     2);
	/* Config TTL sensor format */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_10, TTL_SENSOR_BIT,
			     param->sensor_fmt);
	/* Config TTL clock invert */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_30, VI_CLK_INV,
			     param->clk_inv);
	switch (param->fmt) {
	case TTL_SYNC_PAT_17B_BT1120:
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_FMT_IN,
				     TTL_SYNC_PAT_17B_BT1120);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_BT_FMT_OUT,
				     param->fmt_out);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_VS_BP,
				     9);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_HS_BP,
				     8);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_WD,
				     param->width);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_HT,
				     param->height);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_0,
				     0xFFF);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_1,
				     0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_20, TTL_SYNC_2,
				     0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_VLD,
				     0x800);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_BLK,
				     0xab0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_VLD,
				     0x9d0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_BLK,
				     0xb60);

		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_SEL,
				     param->vi_sel);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_V_SEL_VS,
				     1);
		break;
	case TTL_VHS_19B_BT601:
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_FMT_IN,
				     TTL_VHS_19B_BT601);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_BT_FMT_OUT,
				     param->fmt_out);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_VS_INV,
				     1);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_HS_INV,
				     1);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_VS_BP,
				     0x23);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_HS_BP,
				     0xbf);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_WD,
				     param->width);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_HT,
				     param->height);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_0,
				     0xFFF);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_SEL,
				     param->vi_sel);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_V_SEL_VS,
				     1);
		break;
	case TTL_SYNC_PAT_9B_BT656:
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_FMT_IN,
				     TTL_SYNC_PAT_9B_BT656);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_BT_FMT_OUT,
				     param->fmt_out);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_VS_BP,
				     0x0f);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_HS_BP,
				     0x0f);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_WD,
				     param->width);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_HT,
				     param->height);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_0,
				     0xFFF);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_1,
				     0x0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_20, TTL_SYNC_2,
				     0x0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_VLD,
				     0x800);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_BLK,
				     0xAB0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_VLD,
				     0x9D0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_BLK,
				     0xB60);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_SEL,
				     param->vi_sel);
		break;
	case TTL_CUSTOM_0:
		/* Config TTL format */
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_FMT_IN,
				     TTL_VHS_19B_BT601);
		/* Config TTL format */
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_BT_FMT_OUT,
				     param->fmt_out);
		/* Config HV inverse */
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_VS_INV,
				     1);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_HS_INV,
				     1);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_VS_BP,
				     4095);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_14, TTL_HS_BP,
				     1);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_WD,
				     param->width);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_18, TTL_IMG_HT,
				     param->height);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_0,
				     0xFFF);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_1C, TTL_SYNC_1,
				     0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_20, TTL_SYNC_2,
				     0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_VLD,
				     0x800);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_24, TTL_SAV_BLK,
				     0xab0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_VLD,
				     0x9d0);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_28, TTL_EAV_BLK,
				     0xb60);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_SEL,
				     param->vi_sel);
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_30, VI_V_SEL_VS,
				     1);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				      REG_20, MIPIRX_GPI_FRC_CM,
				      0x0C);
		break;
	default:
		/* Config TTL format */
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_10, TTL_FMT_IN,
				     param->fmt);
		break;
	}
}

static void _cif_csi_config(struct cif_ctx *ctx,
			   struct param_csi *param)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];

	/* Config the sensor mode */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, SENSOR_MAC_MODE,
			     0);
	/* invert the HS/VS */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, CSI_VS_INV,
			     1);
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, CSI_HS_INV,
			     1);
	/* CSI controller enable */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, CSI_CTRL_ENABLE,
			     1);
	/* Config the lane enable */
	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_00, CSI_LANE_MODE,
			     param->lane_num - 1);
	/* Config the VS gen mode */
	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_70, CSI_VS_GEN_MODE,
			     param->vs_gen_mode);
	/* DPHY sensor mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SENSOR_MODE,
			      0);
}

void cif_crop_info_line(struct cif_ctx *ctx, uint32_t line_num, uint32_t sw_up)
{
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];

	/* Config the info line strip for HDR pattern 2 */
	if (line_num) {
		CIF_WR_BITS_GRP2(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_48,
				     SENSOR_MAC_INFO_LINE_NUM,
				     line_num,
				     SENSOR_MAC_RM_INFO_LINE,
				     1);

		if (sw_up) {
			CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
					     REG_00, SW_UP,
					     1);
			CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
					     REG_00, SW_UP,
					     1);
		}
	} else {
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_48,
				     SENSOR_MAC_RM_INFO_LINE,
				     0);
	}
}

void cif_set_bt_fmt_out(struct cif_ctx *ctx, enum ttl_bt_fmt_out fmt_out)
{
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];

	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_10, TTL_BT_FMT_OUT,
			     fmt_out);
}

void cif_config(struct cif_ctx *ctx, struct cif_param *param)
{
	switch (param->type) {
	case CIF_TYPE_CSI:
		_cif_csi_config(ctx, &param->cfg.csi);
		break;
	case CIF_TYPE_SUBLVDS:
		_cif_sublvds_config(ctx, &param->cfg.sublvds);
		break;
	case CIF_TYPE_HISPI:
		_cif_hispi_config(ctx, &param->cfg.hispi);
		break;
	case CIF_TYPE_TTL:
		_cif_ttl_config(ctx, &param->cfg.ttl);
		break;
	default:
		break;
	}
}

static void _cif_hdr_sublvds_enable(struct cif_ctx *ctx,
				    struct param_sublvds *param,
				    uint32_t on)
{
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	if (!on) {
		/*[TODO] V FP recovery? info line strip? */
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_00, SLVDS_HDR_MODE,
				       0);
		return;
	}

	/* Config the HDR. */
	switch (param->hdr_mode) {
	case CIF_SLVDS_HDR_PAT1:
		/* Config the sync code if raw 10, default is raw 12*/
		/* Select the HDR pattern */
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_00, SLVDS_HDR_PATTERN,
				       param->hdr_mode>>1);
		break;
	case CIF_SLVDS_HDR_PAT2:
		/* Config the HSIZE and HBlank per lane */
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_58, SLVDS_HDR_P2_HSIZE,
				       param->h_size/param->lane_num);
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_58, SLVDS_HDR_P2_HBLANK,
				       param->hdr_hblank[0]/param->lane_num);
		/* Select the HDR pattern */
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_00, SLVDS_HDR_PATTERN,
				       param->hdr_mode>>1);
		break;
	}

	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_HDR_MODE,
			       1);
}

static void _cif_hdr_csi_enable(struct cif_ctx *ctx,
				 struct param_csi *param,
				 uint32_t on)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];

	if (param->hdr_mode == CSI_HDR_MODE_VC) {
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_08, CSI_HDR_MODE,
				     0);
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_70, CSI_VS_GEN_BY_VCSET,
				     1);
	} else if (param->hdr_mode == CSI_HDR_MODE_DT) {
		/* Enable dtat type mode. */
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_74, CSI_HDR_DT_MODE,
				     1);
		/* Program lef data type. */
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_74, CSI_HDR_DT_LEF,
				     param->data_type[0]);
		/* Program sef data type. */
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_74, CSI_HDR_DT_SEF,
				     param->data_type[1]);
		/* Program decode data type. */
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_74, CSI_HDR_DT_FORMAT,
				     param->decode_type);
	} else {
		/* [TODO] */
		CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_08, CSI_HDR_MODE,
				     1);
	}
	/* invert the HDR */
	CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			     REG_00, CSI_HDR_INV,
			     1);
	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_08, CSI_HDR_EN,
			     !!on);
}

static void _cif_hdr_hispi_enable(struct cif_ctx *ctx,
				  struct param_hispi *param,
				  uint32_t on)
{
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	if (param->mode == CIF_HISPI_MODE_PKT_SP) {
		CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
				       REG_60, HISPI_HDR_PSP_MODE,
				       !!on);

	}
}

void cif_hdr_manual_config(struct cif_ctx *ctx,
			   struct cif_param *param,
			   uint32_t sw_up)
{
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];

	if (!param->hdr_manual) {
		CIF_WR_BITS_GRP3(mac_top, REG_SENSOR_MAC_1C4D_T,
				 REG_40,
				 SENSOR_MAC_HDR_EN,
				 0,
				 SENSOR_MAC_HDR_HDRINV,
				 0,
				 SENSOR_MAC_HDR_MODE,
				 0);
		if (sw_up) {
			CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
					     REG_00, SW_UP,
					     1);
		}
		return;
	}

	/* Config the HDR mode V size and T2 line shift */
	CIF_WR_BITS_GRP2(mac_top, REG_SENSOR_MAC_1C4D_T,
			 REG_44,
			 SENSOR_MAC_HDR_VSIZE,
			 param->hdr_vsize,
			 SENSOR_MAC_HDR_SHIFT,
			 param->hdr_shift);

	CIF_WR_BITS_GRP3(mac_top, REG_SENSOR_MAC_1C4D_T,
			 REG_40,
			 SENSOR_MAC_HDR_EN,
			 1,
			 SENSOR_MAC_HDR_HDRINV,
			 1,
			 SENSOR_MAC_HDR_MODE,
			 !!param->hdr_rm_padding);
	if (sw_up) {
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
				     REG_00, SW_UP,
				     1);
	}
}

void cif_hdr_enable(struct cif_ctx *ctx, struct cif_param *param, uint32_t on)
{
	if (param->hdr_manual)
		return;

	switch (param->type) {
	case CIF_TYPE_CSI:
		_cif_hdr_csi_enable(ctx, &param->cfg.csi, on);
		break;
	case CIF_TYPE_SUBLVDS:
		_cif_hdr_sublvds_enable(ctx, &param->cfg.sublvds, on);
		break;
	case CIF_TYPE_HISPI:
		_cif_hdr_hispi_enable(ctx, &param->cfg.hispi, on);
		break;
	default:
		break;
	}

}

static void cif_stream_enable(struct cif_ctx *ctx,
			      struct cif_param *param, uint32_t on)
{
	uint64_t mac_top = ctx->mac_phys_regs[CIF_MAC_BLK_ID_TOP];
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	union cif_cfg *cfg = &param->cfg;


	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
		    REG_00, MIPIRX_PD_IBIAS, 1);
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
		    REG_00, MIPIRX_PD_RXLP, 0x1F);
	if (on) {
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			    REG_00, MIPIRX_PD_IBIAS, 0);
	}

	switch (param->type) {
	case CIF_TYPE_CSI:
		/* clear the lane enable */
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			    REG_28, DESKEW_LANE_EN, 0);
		if (on) {
			CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_00, MIPIRX_PD_RXLP, 0);
			usleep(20);
			/* lane enable */
			CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_28, DESKEW_LANE_EN,
				    (1 << cfg->csi.lane_num) - 1);
		}
		break;
	case CIF_TYPE_SUBLVDS:
		/* clear the lane enable */
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			    LVDS_F0, SLVDS_LANE_EN, 0);
		if (on) {
			usleep(20);
			/* lane enable */
			CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    LVDS_F0, SLVDS_LANE_EN,
				    (1 << cfg->sublvds.lane_num) - 1);
		}
		break;
	case CIF_TYPE_HISPI:
		/* clear the lane enable */
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			    LVDS_F0, SLVDS_LANE_EN, 0);
		if (on) {
			usleep(20);
			/* lane enable */
			CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    LVDS_F0, SLVDS_LANE_EN,
				    (1 << cfg->hispi.lane_num) - 1);
		}
		break;
	case CIF_TYPE_TTL:
		/* Enable TTL */
		CIF_WR_BITS(mac_top, REG_SENSOR_MAC_1C4D_T,
			    REG_10, TTL_IP_EN,
			    !!on);
		break;
	default:
		break;
	}

}

void cif_streaming(struct cif_ctx *ctx, uint32_t on, uint32_t hdr)
{
	/* CIF OFF */
	cif_stream_enable(ctx, ctx->cur_config, 0);

	if (on) {
		cif_config(ctx, ctx->cur_config);
		cif_hdr_enable(ctx, ctx->cur_config, hdr);
		/* CIF ON */
		cif_stream_enable(ctx, ctx->cur_config, on);
	}
}

void cif_set_lane_id(struct cif_ctx *ctx, enum lane_id_e lane,
			uint32_t select, uint32_t pn_swap)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	switch (lane) {
	case CIF_LANE_CLK:
		/* PHYA clock select */
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_04, MIPIRX_SEL_CLK_CHANNEL,
			      1 << select);
		/* PHYD clock select */
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_CK_SEL,
			      select);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_CK_PNSWAP,
			      pn_swap);
		break;
	case CIF_LANE_0:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D0_SEL,
			      select);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D0_PNSWAP,
			      pn_swap);
		break;
	case CIF_LANE_1:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D1_SEL,
			      select);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D1_PNSWAP,
			      pn_swap);
		break;
	case CIF_LANE_2:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D2_SEL,
			      select);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D2_PNSWAP,
			      pn_swap);
		break;
	case CIF_LANE_3:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D3_SEL,
			      select);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D3_PNSWAP,
			      pn_swap);
		break;
	default:
		break;
	}
}

void cif_set_clk_edge(struct cif_ctx *ctx,
		      enum lane_id_e lane, enum cif_clk_edge_e edge)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	switch (lane) {
	case CIF_LANE_0:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D0_CKINV,
			      edge);
		break;
	case CIF_LANE_1:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D1_CKINV,
			      edge);
		break;
	case CIF_LANE_2:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D2_CKINV,
			      edge);
		break;
	case CIF_LANE_3:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      REG_28, CSI_LANE_D3_CKINV,
			      edge);
		break;
	default:
		break;
	}
}

void cif_set_hs_settle(struct cif_ctx *ctx, uint8_t hs_settle)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
		      REG_48, T_HS_SETTLE,
		      hs_settle);
}

uint8_t cif_get_lane_data(struct cif_ctx *ctx, enum phy_lane_id_e lane)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	uint8_t value;

	switch (lane) {
	case CIF_PHY_LANE_0:
		value = CIF_RD_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_40, AD_D0_DATA);
		break;
	case CIF_PHY_LANE_1:
		value = CIF_RD_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_40, AD_D1_DATA);
		break;
	case CIF_PHY_LANE_2:
		value = CIF_RD_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_40, AD_D2_DATA);
		break;
	case CIF_PHY_LANE_3:
		value = CIF_RD_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_40, AD_D3_DATA);
		break;
	case CIF_PHY_LANE_4:
		value = CIF_RD_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
				    REG_44, AD_D4_DATA);
		break;
	default:
		value = 0;
		break;
	}

	return value;
}

void cif_set_lane_deskew(struct cif_ctx *ctx,
		      enum phy_lane_id_e lane, uint8_t phase)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	switch (lane) {
	case CIF_PHY_LANE_0:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, DESKEW_CODE0,
			      phase);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, FORCE_DESKEW_CODE0,
			      1);
		break;
	case CIF_PHY_LANE_1:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, DESKEW_CODE1,
			      phase);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, FORCE_DESKEW_CODE1,
			      1);
		break;
	case CIF_PHY_LANE_2:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_64, DESKEW_CODE2,
			      phase);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, FORCE_DESKEW_CODE2,
			      1);
		break;
	case CIF_PHY_LANE_3:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_64, DESKEW_CODE3,
			      phase);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, FORCE_DESKEW_CODE3,
			      1);
		break;
	case CIF_PHY_LANE_4:
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_64, DESKEW_CODE4,
			      phase);
		CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      FRC_60, FORCE_DESKEW_CODE4,
			      1);
		break;
	default:
		break;
	}
}

void cif_set_lvds_endian(struct cif_ctx *ctx,
			 enum cif_endian mac, enum cif_endian wrap)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	/* Config the endian. */
	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_DATA_REVERSE,
			       mac == CIF_SLVDS_ENDIAN_LSB);
	/* DPHY endian mode select */
	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
			      LVDS_F0, SLVDS_INV_EN,
			      wrap == CIF_SLVDS_ENDIAN_MSB);
}

void cif_set_lvds_vsync_gen(struct cif_ctx *ctx, uint32_t fp)
{
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	CIF_WR_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
			       REG_00, SLVDS_VFPORCH,
			       fp);
}

int cif_check_csi_int_sts(struct cif_ctx *ctx, uint32_t mask)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];
	uint32_t reg = CIF_RD_REG(mac_csi, REG_CSI_CTRL_TOP_T, REG_60);

	return !!(reg & mask);
}

void cif_clear_csi_int_sts(struct cif_ctx *ctx)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];

	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_04, CSI_INTR_CLR,
			     0xFF);
}

void cif_mask_csi_int_sts(struct cif_ctx *ctx, uint32_t mask)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];

	(void)mask;
	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_04, CSI_INTR_MASK,
			     0xFF);
}

void cif_unmask_csi_int_sts(struct cif_ctx *ctx, uint32_t mask)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];

	(void)mask;
	CIF_WR_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
			     REG_04, CSI_INTR_MASK,
			     0x00);
}

int cif_check_csi_fifo_full(struct cif_ctx *ctx)
{
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];

	return !!CIF_RD_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				      REG_40, CSI_FIFO_FULL);
}

int cif_check_lvds_fifo_full(struct cif_ctx *ctx)
{
	uint64_t mac_slvds = ctx->mac_phys_regs[CIF_MAC_BLK_ID_SLVDS];

	return !!CIF_RD_BITS(mac_slvds, REG_SUBLVDS_CTRL_TOP_T,
					REG_40, SLVDS_FIFO_FULL);
}

int cif_get_csi_decode_fmt(struct cif_ctx *ctx)
{
	int i;
	uint64_t mac_csi = ctx->mac_phys_regs[CIF_MAC_BLK_ID_CSI];
	uint32_t value = CIF_RD_BITS(mac_csi, REG_CSI_CTRL_TOP_T,
				     REG_40, CSI_DECODE_FORMAT);

	for (i = 0; i < DEC_FMT_NUM; i++) {
		if (value & (1 << i))
			return i;
	}

	return i;
}

int cif_get_csi_phy_state(struct cif_ctx *ctx, union mipi_phy_state *state)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	state->raw = CIF_RD_REG(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T, DBG_300);

	return 0;
}

void cif_set_lvds_en(struct cif_ctx *ctx, uint8_t lane_num)
{
	uint64_t wrap_top = ctx->wrap_phys_regs[CIF_WRAP_BLK_ID_TOP];

	CIF_WR_BITS(wrap_top, REG_CSI_DPHY_4LANE_WRAP_T,
		    LVDS_F0, SLVDS_LANE_EN,
		    (1 << lane_num) - 1);
}

uint64_t *cif_get_mac_phys_reg_bases(uint32_t link)
{
	m_cif_mac_phys_base_list[link][CIF_MAC_BLK_ID_TOP] =
					(CIF_MAC_BLK_BA_TOP);
	m_cif_mac_phys_base_list[link][CIF_MAC_BLK_ID_SLVDS] =
					(CIF_MAC_BLK_BA_SLVDS);
	m_cif_mac_phys_base_list[link][CIF_MAC_BLK_ID_CSI] =
					(CIF_MAC_BLK_BA_CSI);


	return m_cif_mac_phys_base_list[link];
}

uint64_t *cif_get_wrap_phys_reg_bases(uint32_t link)
{
	m_cif_wrap_phys_base_list[link][CIF_WRAP_BLK_ID_TOP] =
					(CIF_WRAP_BLK_BA_TOP);

	return m_cif_wrap_phys_base_list[link];
}

