#ifndef _DISP_TIMING_H_
#define _DISP_TIMING_H_

enum sclr_panel {
	SCL_PANEL_TTL,
	SCL_PANEL_720x1280,
	SCL_PANEL_1080P,
	SCL_PANEL_MAX,
};

struct sclr_disp_timing panels[] = {
	{	// ttl/lvds	    800x600
		.vsync_pol = false,
		.hsync_pol = true,
		.vtotal = 628,
		.htotal = 831,
		.vsync_start = 1,
		.vsync_end = 4,
		.vfde_start = 28,
		.vfde_end = 627,
		.vmde_start = 28,
		.vmde_end = 627,
		.hsync_start = 1,
		.hsync_end = 10,
		.hfde_start = 15,
		.hfde_end = 815,
		.hmde_start = 15,
		.hmde_end = 815,
	},
	#if 0
	{	// dsi ili9881	    720*1280
		.vsync_pol = false,
		.hsync_pol = true,
		.vtotal = 1649,
		.htotal = 899,
		.vsync_start = 1,
		.vsync_end = 10,
		.vfde_start = 28,
		.vfde_end = 1307,
		.vmde_start = 28,
		.vmde_end = 1307,
		.hsync_start = 1,
		.hsync_end = 10,
		.hfde_start = 15,
		.hfde_end = 734,
		.hmde_start = 15,
		.hmde_end = 734,
	},
	#else
	{	// hx8394	    720*1280
		.vsync_pol = false,
		.hsync_pol = true,
		.vtotal = 1305,
		.htotal = 947,
		.vsync_start = 1,
		.vsync_end = 16,
		.vfde_start = 20,
		.vfde_end = 1299,
		.vmde_start = 20,
		.vmde_end = 1299,
		.hsync_start = 1,
		.hsync_end = 64,
		.hfde_start = 100,
		.hfde_end = 819,
		.hmde_start = 100,
		.hmde_end = 819,
	},
	#endif
	{	// PXP 1080P
		.vsync_pol = true,
		.hsync_pol = true,
		.vtotal = 1125,
		.htotal = 2200,
		.vsync_start = 1,
		.vsync_end = 6,
		.vfde_start = 41,
		.vfde_end = 1120,
		.vmde_start = 41,
		.vmde_end = 1120,
		.hsync_start = 1,
		.hsync_end = 45,
		.hfde_start = 133,
		.hfde_end = 2052,
		.hmde_start = 133,
		.hmde_end = 2052,
	},
};

#endif // _DISP_TIMING_H_

