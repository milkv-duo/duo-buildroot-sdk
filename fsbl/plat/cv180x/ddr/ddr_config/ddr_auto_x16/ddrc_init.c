#include <mmio.h>
// #include <soc.h>
#include <reg_soc.h>
#include <ddr_sys.h>
#ifdef DDR2_3
#include <ddr3_1866_init.h>
#include <ddr2_1333_init.h>
#else
#include <ddr_init.h>
#endif
#include <bitwise_ops.h>
#include <ddr_pkg_info.h>

uint32_t ddr_data_rate = 1866;

void ddrc_init(void)
{
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		NOTICE("DDR3 1866 ddrc_init\n");
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		// PATCH0.use_blk_ext}:0:2:=0x1
		// PATCH0.dis_auto_ref_cnt_fix:2:1:=0x0
		// PATCH0.dis_auto_ref_algn_to_8:3:1:=0x0
		// PATCH0.starve_stall_at_dfi_ctrlupd:4:1:=0x1
		// PATCH0.starve_stall_at_abr:5:1:=0x1
		// PATCH0.dis_rdwr_switch_at_abr:6:1:=0x1
		// PATCH0.dfi_wdata_same_to_axi:7:1:=0x0
		// PATCH0.pagematch_limit_threshold:8:3=0x3
		// PATCH0.qos_sel:12:2:=0x2
		// PATCH0.burst_rdwr_xpi:16:4:=0x4
		// PATCH0.always_critical_when_urgent_hpr:20:1:=0x1
		// PATCH0.always_critical_when_urgent_lpr:21:1:=0x1
		// PATCH0.always_critical_when_urgent_wr:22:1:=0x1
		// PATCH0.disable_hif_rcmd_stall_path:24:1:=0x1
		// PATCH0.disable_hif_wcmd_stall_path:25:1:=0x1
		// PATCH0.derate_sys_en:29:1:=0x1
		// PATCH0.ref_4x_sys_high_temp:30:1:=0x1
		mmio_wr32(0x08004000 + 0x44, 0x14000000);
		// PATCH1.ref_adv_stop_threshold:0:7:=0x0
		// PATCH1.ref_adv_dec_threshold:8:7:=0x0
		// PATCH1.ref_adv_max:16:7:=0x0
		// PATCH1.burst_rdwr_wr_xpi:24:4:=0x4
		// PATCH1.use_blk_extend:28:2:=0x1
		mmio_wr32(0x08004000 + 0x6c, 0x00000003);
		// PATCH5.vpr_fix:0:1:=0x1
		// PATCH5.vpw_fix:1:1:=0x1
		mmio_wr32(0x08004000 + 0x148, 0x999F0000);
		// PATCH4.t_phyd_rden:16:6=0x0
		// PATCH4.phyd_rd_clk_stop:23:1=0x0
		// PATCH4.t_phyd_wren:24:6=0x0
		// PATCH4.phyd_wr_clk_stop:31:1=0x0
		// auto gen.
		mmio_wr32(0x08004000 + 0x0, 0x81041401);
		mmio_wr32(0x08004000 + 0x30, 0x00000000);
		mmio_wr32(0x08004000 + 0x34, 0x00930001);
		mmio_wr32(0x08004000 + 0x38, 0x00020000);
		mmio_wr32(0x08004000 + 0x50, 0x00201070);
		mmio_wr32(0x08004000 + 0x60, 0x00000000);
		mmio_wr32(0x08004000 + 0x64, 0x007100A4);
		mmio_wr32(0x08004000 + 0xc0, 0x00000000);
		mmio_wr32(0x08004000 + 0xc4, 0x00000000);
	#ifdef DDR_INIT_SPEED_UP
		mmio_wr32(0x08004000 + 0xd0, 0x00010002);
		mmio_wr32(0x08004000 + 0xd4, 0x00020000);
	#else
		mmio_wr32(0x08004000 + 0xd0, 0x000100E5);
		mmio_wr32(0x08004000 + 0xd4, 0x006A0000);
	#endif
		mmio_wr32(0x08004000 + 0xdc, 0x1F140040);
	#ifdef DDR_DODT
		mmio_wr32(0x08004000 + 0xe0, 0x04600000);
	#else
		mmio_wr32(0x08004000 + 0xe0, 0x00600000);
	#endif
		mmio_wr32(0x08004000 + 0xe4, 0x000B03BF);
		mmio_wr32(0x08004000 + 0x100, 0x0E111F10);
		mmio_wr32(0x08004000 + 0x104, 0x00030417);
		mmio_wr32(0x08004000 + 0x108, 0x0507060A);
		mmio_wr32(0x08004000 + 0x10c, 0x00002007);
		mmio_wr32(0x08004000 + 0x110, 0x07020307);
		mmio_wr32(0x08004000 + 0x114, 0x05050303);
		mmio_wr32(0x08004000 + 0x120, 0x00000907);
		mmio_wr32(0x08004000 + 0x13c, 0x00000000);
		mmio_wr32(0x08004000 + 0x180, 0xC0960026);
		mmio_wr32(0x08004000 + 0x184, 0x00000001);
		// phyd related
		mmio_wr32(0x08004000 + 0x190, 0x048a8305);
		// DFITMG0.dfi_t_ctrl_delay:24:5:=0x4
		// DFITMG0.dfi_rddata_use_dfi_phy_clk:23:1:=0x1
		// DFITMG0.dfi_t_rddata_en:16:7:=0xa
		// DFITMG0.dfi_wrdata_use_dfi_phy_clk:15:1:=0x1
		// DFITMG0.dfi_tphy_wrdata:8:6:=0x3
		// DFITMG0.dfi_tphy_wrlat:0:6:=0x5
		mmio_wr32(0x08004000 + 0x194, 0x00070202);
		// DFITMG1.dfi_t_cmd_lat:28:4:=0x0
		// DFITMG1.dfi_t_parin_lat:24:2:=0x0
		// DFITMG1.dfi_t_wrdata_delay:16:5:=0x7
		// DFITMG1.dfi_t_dram_clk_disable:8:5:=0x2
		// DFITMG1.dfi_t_dram_clk_enable:0:5:=0x2
		mmio_wr32(0x08004000 + 0x198, 0x07c13121);
		// DFILPCFG0.dfi_tlp_resp:24:5:=0x7
		// DFILPCFG0.dfi_lp_wakeup_dpd:20:4:=0xc
		// DFILPCFG0.dfi_lp_en_dpd:16:1:=0x1
		// DFILPCFG0.dfi_lp_wakeup_sr:12:4:=0x3
		// DFILPCFG0.dfi_lp_en_sr:8:1:=0x1
		// DFILPCFG0.dfi_lp_wakeup_pd:4:4:=0x2
		// DFILPCFG0.dfi_lp_en_pd:0:1:=0x1
		mmio_wr32(0x08004000 + 0x19c, 0x00000021);
		// DFILPCFG1.dfi_lp_wakeup_mpsm:4:4:=0x2
		// DFILPCFG1.dfi_lp_en_mpsm:0:1:=0x1
		// auto gen.
		mmio_wr32(0x08004000 + 0x1a0, 0xC0400018);
		mmio_wr32(0x08004000 + 0x1a4, 0x00FE00FF);
		mmio_wr32(0x08004000 + 0x1a8, 0x80000000);
		mmio_wr32(0x08004000 + 0x1b0, 0x000002C1);
		mmio_wr32(0x08004000 + 0x1c0, 0x00000001);
		mmio_wr32(0x08004000 + 0x1c4, 0x00000001);
		// address map, auto gen.
		mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
		mmio_wr32(0x08004000 + 0x204, 0x00070707);
		mmio_wr32(0x08004000 + 0x208, 0x00000000);
		mmio_wr32(0x08004000 + 0x20c, 0x1F000000);
		mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
		mmio_wr32(0x08004000 + 0x214, 0x060F0606);
		mmio_wr32(0x08004000 + 0x218, 0x06060606);
		mmio_wr32(0x08004000 + 0x21c, 0x00000606);
		mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
		mmio_wr32(0x08004000 + 0x224, 0x06060606);
		mmio_wr32(0x08004000 + 0x228, 0x06060606);
		mmio_wr32(0x08004000 + 0x22c, 0x001F1F06);
		// auto gen.
		mmio_wr32(0x08004000 + 0x240, 0x08000610);
	#ifdef DDR_DODT
		mmio_wr32(0x08004000 + 0x244, 0x00000001);
	#else
		mmio_wr32(0x08004000 + 0x244, 0x00000000);
	#endif
		mmio_wr32(0x08004000 + 0x250, 0x00003F85);
		// SCHED.opt_vprw_sch:31:1:=0x0
		// SCHED.rdwr_idle_gap:24:7:=0x0
		// SCHED.go2critical_hysteresis:16:8:=0x0
		// SCHED.lpddr4_opt_act_timing:15:1:=0x0
		// SCHED.lpr_num_entries:8:7:=0x1f
		// SCHED.autopre_rmw:7:1:=0x1
		// SCHED.dis_opt_ntt_by_pre:6:1:=0x0
		// SCHED.dis_opt_ntt_by_act:5:1:=0x0
		// SCHED.opt_wrcam_fill_level:4:1:=0x0
		// SCHED.rdwr_switch_policy_sel:3:1:=0x0
		// SCHED.pageclose:2:1:=0x1
		// SCHED.prefer_write:1:1:=0x0
		// SCHED.dis_opt_wrecc_collision_flush:0:1:=0x1
		mmio_wr32(0x08004000 + 0x254, 0x00000000);
		// SCHED1.page_hit_limit_rd:28:3:=0x0
		// SCHED1.page_hit_limit_wr:24:3:=0x0
		// SCHED1.visible_window_limit_rd:20:3:=0x0
		// SCHED1.visible_window_limit_wr:16:3:=0x0
		// SCHED1.delay_switch_write:12:4:=0x0
		// SCHED1.pageclose_timer:0:8:=0x0
		// auto gen.
		mmio_wr32(0x08004000 + 0x25c, 0x100000F0);
		// PERFHPR1.hpr_xact_run_length:24:8:=0x20
		// PERFHPR1.hpr_max_starve:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x264, 0x100000F0);
		// PERFLPR1.lpr_xact_run_length:24:8:=0x20
		// PERFLPR1.lpr_max_starve:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x26c, 0x100000F0);
		// PERFWR1.w_xact_run_length:24:8:=0x20
		// PERFWR1.w_max_starve:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x300, 0x00000000);
		// DBG0.dis_max_rank_wr_opt:7:1:=0x0
		// DBG0.dis_max_rank_rd_opt:6:1:=0x0
		// DBG0.dis_collision_page_opt:4:1:=0x0
		// DBG0.dis_act_bypass:2:1:=0x0
		// DBG0.dis_rd_bypass:1:1:=0x0
		// DBG0.dis_wc:0:1:=0x0
		mmio_wr32(0x08004000 + 0x304, 0x00000000);
		// DBG1.dis_hif:1:1:=0x0
		// DBG1.dis_dq:0:1:=0x0
		mmio_wr32(0x08004000 + 0x30c, 0x00000000);
		mmio_wr32(0x08004000 + 0x320, 0x00000001);
		// SWCTL.sw_done:0:1:=0x1
		mmio_wr32(0x08004000 + 0x36c, 0x00000000);
		// POISONCFG.rd_poison_intr_clr:24:1:=0x0
		// POISONCFG.rd_poison_intr_en:20:1:=0x0
		// POISONCFG.rd_poison_slverr_en:16:1:=0x0
		// POISONCFG.wr_poison_intr_clr:8:1:=0x0
		// POISONCFG.wr_poison_intr_en:4:1:=0x0
		// POISONCFG.wr_poison_slverr_en:0:1:=0x0
		mmio_wr32(0x08004000 + 0x400, 0x00000011);
		// PCCFG.dch_density_ratio:12:2:=0x0
		// PCCFG.bl_exp_mode:8:1:=0x0
		// PCCFG.pagematch_limit:4:1:=0x1
		// PCCFG.go2critical_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x404, 0x00006000);
		// PCFGR_0.rdwr_ordered_en:16:1:=0x0
		// PCFGR_0.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_0.rd_port_urgent_en:13:1:=0x1
		// PCFGR_0.rd_port_aging_en:12:1:=0x0
		// PCFGR_0.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_0.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x408, 0x00006000);
		// PCFGW_0.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_0.wr_port_urgent_en:13:1:=0x1
		// PCFGW_0.wr_port_aging_en:12:1:=0x0
		// PCFGW_0.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x490, 0x00000001);
		// PCTRL_0.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x494, 0x00000007);
		// PCFGQOS0_0.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_0.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_0.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_0.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_0.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x498, 0x0000006a);
		// PCFGQOS1_0.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_0.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x49c, 0x00000e07);
		// PCFGWQOS0_0.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_0.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_0.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_0.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_0.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x4a0, 0x01a801a8);
		// PCFGWQOS1_0.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_0.wqos_map_timeout1:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x4b4, 0x00006000);
		// PCFGR_1.rdwr_ordered_en:16:1:=0x0
		// PCFGR_1.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_1.rd_port_urgent_en:13:1:=0x1
		// PCFGR_1.rd_port_aging_en:12:1:=0x0
		// PCFGR_1.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_1.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x4b8, 0x00006000);
		// PCFGW_1.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_1.wr_port_urgent_en:13:1:=0x1
		// PCFGW_1.wr_port_aging_en:12:1:=0x0
		// PCFGW_1.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x540, 0x00000001);
		// PCTRL_1.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x544, 0x00000007);
		// PCFGQOS0_1.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_1.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_1.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_1.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_1.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x548, 0x0000006a);
		// PCFGQOS1_1.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_1.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x54c, 0x00000e07);
		// PCFGWQOS0_1.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_1.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_1.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_1.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_1.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x550, 0x01a801a8);
		// PCFGWQOS1_1.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_1.wqos_map_timeout1:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x564, 0x00006000);
		// PCFGR_2.rdwr_ordered_en:16:1:=0x0
		// PCFGR_2.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_2.rd_port_urgent_en:13:1:=0x1
		// PCFGR_2.rd_port_aging_en:12:1:=0x0
		// PCFGR_2.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_2.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x568, 0x00006000);
		// PCFGW_2.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_2.wr_port_urgent_en:13:1:=0x1
		// PCFGW_2.wr_port_aging_en:12:1:=0x0
		// PCFGW_2.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x5f0, 0x00000001);
		// PCTRL_2.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x5f4, 0x00000007);
		// PCFGQOS0_2.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_2.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_2.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_2.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_2.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x5f8, 0x0000006a);
		// PCFGQOS1_2.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_2.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x5fc, 0x00000e07);
		// PCFGWQOS0_2.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_2.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_2.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_2.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_2.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x600, 0x01a801a8);
		// PCFGWQOS1_2.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_2.wqos_map_timeout1:0:16:=0x1a8
	} else {
		NOTICE("DDR2 1333 ddrc_init\n");
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		// PATCH0.use_blk_ext}:0:2:=0x1
		// PATCH0.dis_auto_ref_cnt_fix:2:1:=0x0
		// PATCH0.dis_auto_ref_algn_to_8:3:1:=0x0
		// PATCH0.starve_stall_at_dfi_ctrlupd:4:1:=0x1
		// PATCH0.starve_stall_at_abr:5:1:=0x1
		// PATCH0.dis_rdwr_switch_at_abr:6:1:=0x1
		// PATCH0.dfi_wdata_same_to_axi:7:1:=0x0
		// PATCH0.pagematch_limit_threshold:8:3=0x3
		// PATCH0.qos_sel:12:2:=0x2
		// PATCH0.burst_rdwr_xpi:16:4:=0x4
		// PATCH0.always_critical_when_urgent_hpr:20:1:=0x1
		// PATCH0.always_critical_when_urgent_lpr:21:1:=0x1
		// PATCH0.always_critical_when_urgent_wr:22:1:=0x1
		// PATCH0.disable_hif_rcmd_stall_path:24:1:=0x1
		// PATCH0.disable_hif_wcmd_stall_path:25:1:=0x1
		// PATCH0.derate_sys_en:29:1:=0x1
		// PATCH0.ref_4x_sys_high_temp:30:1:=0x1
		mmio_wr32(0x08004000 + 0x44, 0x14000000);
		// PATCH1.ref_adv_stop_threshold:0:7:=0x0
		// PATCH1.ref_adv_dec_threshold:8:7:=0x0
		// PATCH1.ref_adv_max:16:7:=0x0
		// PATCH1.burst_rdwr_wr_xpi:24:4:=0x4
		// PATCH1.use_blk_extend:28:2:=0x1
		mmio_wr32(0x08004000 + 0x6c, 0x00000003);
		// PATCH5.vpr_fix:0:1:=0x1
		// PATCH5.vpw_fix:1:1:=0x1
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2)
			mmio_wr32(0x08004000 + 0x148, 0x989D0000);
		else
			mmio_wr32(0x08004000 + 0x148, 0x979C0000);

		// PATCH4.t_phyd_rden:16:6=0x0
		// PATCH4.phyd_rd_clk_stop:23:1=0x0
		// PATCH4.t_phyd_wren:24:6=0x0
		// PATCH4.phyd_wr_clk_stop:31:1=0x0
		// auto gen.
		mmio_wr32(0x08004000 + 0x0, 0x81041400);
		mmio_wr32(0x08004000 + 0x30, 0x00000000);
		mmio_wr32(0x08004000 + 0x34, 0x006A0001);
		mmio_wr32(0x08004000 + 0x38, 0x00020000);
		mmio_wr32(0x08004000 + 0x50, 0x00201070);
		mmio_wr32(0x08004000 + 0x60, 0x00000000);
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2)
			mmio_wr32(0x08004000 + 0x64, 0x00510023);
		else
			mmio_wr32(0x08004000 + 0x64, 0x0051002C);

	#ifdef DDR_INIT_SPEED_UP
		mmio_wr32(0x08004000 + 0xd0, 0x00010002);
		mmio_wr32(0x08004000 + 0xd4, 0x00000000);
	#else
		mmio_wr32(0x08004000 + 0xd0, 0x00010043);
		mmio_wr32(0x08004000 + 0xd4, 0x00000000);
	#endif
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2) {
			mmio_wr32(0x08004000 + 0xdc, 0x03030040);
			mmio_wr32(0x08004000 + 0xe0, 0x00800000);
			mmio_wr32(0x08004000 + 0x100, 0x0B01160F);
			mmio_wr32(0x08004000 + 0x104, 0x00020414);
			mmio_wr32(0x08004000 + 0x108, 0x04050409);
			mmio_wr32(0x08004000 + 0x10c, 0x00001004);
			mmio_wr32(0x08004000 + 0x110, 0x05020405);
			mmio_wr32(0x08004000 + 0x114, 0x01010202);
			mmio_wr32(0x08004000 + 0x120, 0x00000503);
		} else {
			mmio_wr32(0x08004000 + 0xdc, 0x03730040);
			mmio_wr32(0x08004000 + 0xe0, 0x00800000);
			mmio_wr32(0x08004000 + 0x100, 0x0A011610);
			mmio_wr32(0x08004000 + 0x104, 0x00030414);
			mmio_wr32(0x08004000 + 0x108, 0x03040408);
			mmio_wr32(0x08004000 + 0x10c, 0x00003004);
			mmio_wr32(0x08004000 + 0x110, 0x05020406);
			mmio_wr32(0x08004000 + 0x114, 0x01010303);
			mmio_wr32(0x08004000 + 0x120, 0x00000503);
		}
		// phyd related
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2)
			mmio_wr32(0x08004000 + 0x190, 0x04878304);
		else
			mmio_wr32(0x08004000 + 0x190, 0x04858302);

		// DFITMG0.dfi_t_ctrl_delay:24:5:=0x4
		// DFITMG0.dfi_rddata_use_dfi_phy_clk:23:1:=0x1
		// DFITMG0.dfi_t_rddata_en:16:7:=0xa
		// DFITMG0.dfi_wrdata_use_dfi_phy_clk:15:1:=0x1
		// DFITMG0.dfi_tphy_wrdata:8:6:=0x3
		// DFITMG0.dfi_tphy_wrlat:0:6:=0x5
		mmio_wr32(0x08004000 + 0x194, 0x00070102);
		// DFITMG1.dfi_t_cmd_lat:28:4:=0x0
		// DFITMG1.dfi_t_parin_lat:24:2:=0x0
		// DFITMG1.dfi_t_wrdata_delay:16:5:=0x7
		// DFITMG1.dfi_t_dram_clk_disable:8:5:=0x2
		// DFITMG1.dfi_t_dram_clk_enable:0:5:=0x2
		mmio_wr32(0x08004000 + 0x198, 0x07c13121);
		// DFILPCFG0.dfi_tlp_resp:24:5:=0x7
		// DFILPCFG0.dfi_lp_wakeup_dpd:20:4:=0xc
		// DFILPCFG0.dfi_lp_en_dpd:16:1:=0x1
		// DFILPCFG0.dfi_lp_wakeup_sr:12:4:=0x3
		// DFILPCFG0.dfi_lp_en_sr:8:1:=0x1
		// DFILPCFG0.dfi_lp_wakeup_pd:4:4:=0x2
		// DFILPCFG0.dfi_lp_en_pd:0:1:=0x1
		mmio_wr32(0x08004000 + 0x19c, 0x00000021);
		// DFILPCFG1.dfi_lp_wakeup_mpsm:4:4:=0x2
		// DFILPCFG1.dfi_lp_en_mpsm:0:1:=0x1
		// auto gen.
		mmio_wr32(0x08004000 + 0x1a0, 0xC0400018);
		mmio_wr32(0x08004000 + 0x1a4, 0x00FE00FF);
		mmio_wr32(0x08004000 + 0x1a8, 0x80000000);
		mmio_wr32(0x08004000 + 0x1b0, 0x000002C1);
		mmio_wr32(0x08004000 + 0x1c0, 0x00000001);
		mmio_wr32(0x08004000 + 0x1c4, 0x00000001);
		// address map, auto gen.
		// support from 0.5Gb to 4Gb
		// R[17:13]B[2]R[12:0]B[1:0]C[9:0]
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2) {
			mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x204, 0x00141414);
			mmio_wr32(0x08004000 + 0x208, 0x00000000);
			mmio_wr32(0x08004000 + 0x20c, 0x1F000000);
			mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x214, 0x030F0303);
			mmio_wr32(0x08004000 + 0x218, 0x06060603);
			mmio_wr32(0x08004000 + 0x21c, 0x00000606);
			mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
			mmio_wr32(0x08004000 + 0x224, 0x03030303);
			mmio_wr32(0x08004000 + 0x228, 0x03030303);
			mmio_wr32(0x08004000 + 0x22c, 0x001F1F03);
		} else {
			mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x204, 0x00140707);
			mmio_wr32(0x08004000 + 0x208, 0x00000000);
			mmio_wr32(0x08004000 + 0x20c, 0x1F000000);
			mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x214, 0x050F0505);
			mmio_wr32(0x08004000 + 0x218, 0x06060605);
			mmio_wr32(0x08004000 + 0x21c, 0x00000606);
			mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
			mmio_wr32(0x08004000 + 0x224, 0x05050505);
			mmio_wr32(0x08004000 + 0x228, 0x05050505);
			mmio_wr32(0x08004000 + 0x22c, 0x001F1F05);
		}
		// auto gen.
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2)
			mmio_wr32(0x08004000 + 0x240, 0x07030710);
		else
			mmio_wr32(0x08004000 + 0x240, 0x07010708);  //TBD

		mmio_wr32(0x08004000 + 0x244, 0x00000000);
		mmio_wr32(0x08004000 + 0x250, 0x00003F85);
		// SCHED.opt_vprw_sch:31:1:=0x0
		// SCHED.rdwr_idle_gap:24:7:=0x0
		// SCHED.go2critical_hysteresis:16:8:=0x0
		// SCHED.lpddr4_opt_act_timing:15:1:=0x0
		// SCHED.lpr_num_entries:8:7:=0x1f
		// SCHED.autopre_rmw:7:1:=0x1
		// SCHED.dis_opt_ntt_by_pre:6:1:=0x0
		// SCHED.dis_opt_ntt_by_act:5:1:=0x0
		// SCHED.opt_wrcam_fill_level:4:1:=0x0
		// SCHED.rdwr_switch_policy_sel:3:1:=0x0
		// SCHED.pageclose:2:1:=0x1
		// SCHED.prefer_write:1:1:=0x0
		// SCHED.dis_opt_wrecc_collision_flush:0:1:=0x1
		mmio_wr32(0x08004000 + 0x254, 0x00000020);
		// SCHED1.page_hit_limit_rd:28:3:=0x0
		// SCHED1.page_hit_limit_wr:24:3:=0x0
		// SCHED1.visible_window_limit_rd:20:3:=0x0
		// SCHED1.visible_window_limit_wr:16:3:=0x0
		// SCHED1.delay_switch_write:12:4:=0x0
		// SCHED1.pageclose_timer:0:8:=0x0
		// auto gen.
		mmio_wr32(0x08004000 + 0x25c, 0x100000A8);
		// PERFHPR1.hpr_xact_run_length:24:8:=0x20
		// PERFHPR1.hpr_max_starve:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x264, 0x100000A8);
		// PERFLPR1.lpr_xact_run_length:24:8:=0x20
		// PERFLPR1.lpr_max_starve:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x26c, 0x100000A8);
		// PERFWR1.w_xact_run_length:24:8:=0x20
		// PERFWR1.w_max_starve:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x300, 0x00000000);
		// DBG0.dis_max_rank_wr_opt:7:1:=0x0
		// DBG0.dis_max_rank_rd_opt:6:1:=0x0
		// DBG0.dis_collision_page_opt:4:1:=0x0
		// DBG0.dis_act_bypass:2:1:=0x0
		// DBG0.dis_rd_bypass:1:1:=0x0
		// DBG0.dis_wc:0:1:=0x0
		mmio_wr32(0x08004000 + 0x304, 0x00000000);
		// DBG1.dis_hif:1:1:=0x0
		// DBG1.dis_dq:0:1:=0x0
		mmio_wr32(0x08004000 + 0x30c, 0x00000000);
		mmio_wr32(0x08004000 + 0x320, 0x00000001);
		// SWCTL.sw_done:0:1:=0x1
		mmio_wr32(0x08004000 + 0x36c, 0x00000000);
		// POISONCFG.rd_poison_intr_clr:24:1:=0x0
		// POISONCFG.rd_poison_intr_en:20:1:=0x0
		// POISONCFG.rd_poison_slverr_en:16:1:=0x0
		// POISONCFG.wr_poison_intr_clr:8:1:=0x0
		// POISONCFG.wr_poison_intr_en:4:1:=0x0
		// POISONCFG.wr_poison_slverr_en:0:1:=0x0
		mmio_wr32(0x08004000 + 0x400, 0x00000011);
		// PCCFG.dch_density_ratio:12:2:=0x0
		// PCCFG.bl_exp_mode:8:1:=0x0
		// PCCFG.pagematch_limit:4:1:=0x1
		// PCCFG.go2critical_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x404, 0x00006000);
		// PCFGR_0.rdwr_ordered_en:16:1:=0x0
		// PCFGR_0.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_0.rd_port_urgent_en:13:1:=0x1
		// PCFGR_0.rd_port_aging_en:12:1:=0x0
		// PCFGR_0.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_0.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x408, 0x00006000);
		// PCFGW_0.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_0.wr_port_urgent_en:13:1:=0x1
		// PCFGW_0.wr_port_aging_en:12:1:=0x0
		// PCFGW_0.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x490, 0x00000001);
		// PCTRL_0.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x494, 0x00000007);
		// PCFGQOS0_0.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_0.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_0.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_0.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_0.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x498, 0x0000006a);
		// PCFGQOS1_0.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_0.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x49c, 0x00000e07);
		// PCFGWQOS0_0.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_0.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_0.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_0.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_0.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x4a0, 0x01a801a8);
		// PCFGWQOS1_0.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_0.wqos_map_timeout1:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x4b4, 0x00006000);
		// PCFGR_1.rdwr_ordered_en:16:1:=0x0
		// PCFGR_1.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_1.rd_port_urgent_en:13:1:=0x1
		// PCFGR_1.rd_port_aging_en:12:1:=0x0
		// PCFGR_1.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_1.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x4b8, 0x00006000);
		// PCFGW_1.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_1.wr_port_urgent_en:13:1:=0x1
		// PCFGW_1.wr_port_aging_en:12:1:=0x0
		// PCFGW_1.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x540, 0x00000001);
		// PCTRL_1.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x544, 0x00000007);
		// PCFGQOS0_1.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_1.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_1.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_1.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_1.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x548, 0x0000006a);
		// PCFGQOS1_1.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_1.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x54c, 0x00000e07);
		// PCFGWQOS0_1.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_1.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_1.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_1.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_1.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x550, 0x01a801a8);
		// PCFGWQOS1_1.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_1.wqos_map_timeout1:0:16:=0x1a8
		mmio_wr32(0x08004000 + 0x564, 0x00006000);
		// PCFGR_2.rdwr_ordered_en:16:1:=0x0
		// PCFGR_2.rd_port_pagematch_en:14:1:=0x1
		// PCFGR_2.rd_port_urgent_en:13:1:=0x1
		// PCFGR_2.rd_port_aging_en:12:1:=0x0
		// PCFGR_2.read_reorder_bypass_en:11:1:=0x0
		// PCFGR_2.rd_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x568, 0x00006000);
		// PCFGW_2.wr_port_pagematch_en:14:1:=0x1
		// PCFGW_2.wr_port_urgent_en:13:1:=0x1
		// PCFGW_2.wr_port_aging_en:12:1:=0x0
		// PCFGW_2.wr_port_priority:0:10:=0x0
		mmio_wr32(0x08004000 + 0x5f0, 0x00000001);
		// PCTRL_2.port_en:0:1:=0x1
		mmio_wr32(0x08004000 + 0x5f4, 0x00000007);
		// PCFGQOS0_2.rqos_map_region2:24:8:=0x0
		// PCFGQOS0_2.rqos_map_region1:20:4:=0x0
		// PCFGQOS0_2.rqos_map_region0:16:4:=0x0
		// PCFGQOS0_2.rqos_map_level2:8:8:=0x0
		// PCFGQOS0_2.rqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x5f8, 0x0000006a);
		// PCFGQOS1_2.rqos_map_timeoutr:16:16:=0x0
		// PCFGQOS1_2.rqos_map_timeoutb:0:16:=0x6a
		mmio_wr32(0x08004000 + 0x5fc, 0x00000e07);
		// PCFGWQOS0_2.wqos_map_region2:24:8:=0x0
		// PCFGWQOS0_2.wqos_map_region1:20:4:=0x0
		// PCFGWQOS0_2.wqos_map_region0:16:4:=0x0
		// PCFGWQOS0_2.wqos_map_level2:8:8:=0xe
		// PCFGWQOS0_2.wqos_map_level1:0:8:=0x7
		mmio_wr32(0x08004000 + 0x600, 0x01a801a8);
		// PCFGWQOS1_2.wqos_map_timeout2:16:16:=0x1a8
		// PCFGWQOS1_2.wqos_map_timeout1:0:16:=0x1a8
	}
}

void ctrl_init_high_patch(void)
{
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// enable auto PD/SR
		mmio_wr32(0x08004000 + 0x30, 0x00000002);
		// enable auto ctrl_upd
		mmio_wr32(0x08004000 + 0x1a0, 0x00400018);
		// enable clock gating
		mmio_wr32(0x0800a000 + 0x14, 0x00000000);
		// change xpi to multi DDR burst
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		mmio_wr32(0x08004000 + 0x44, 0x08000000);
	} else {
		// enable auto PD
		mmio_wr32(0x08004000 + 0x30, 0x00000002);
		// enable auto ctrl_upd
		mmio_wr32(0x08004000 + 0x1a0, 0x00400018);
		// enable clock gating
		mmio_wr32(0x0800a000 + 0x14, 0x00000000);
		// change xpi to multi DDR burst
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		mmio_wr32(0x08004000 + 0x44, 0x08000000);
	}
}

void ctrl_init_low_patch(void)
{
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// disable auto PD/SR
		mmio_wr32(0x08004000 + 0x30, 0x00000000);
		// disable auto ctrl_upd
		mmio_wr32(0x08004000 + 0x1a0, 0xC0400018);
		// disable clock gating
		mmio_wr32(0x0800a000 + 0x14, 0x00000fff);
		// change xpi to single DDR burst
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		mmio_wr32(0x08004000 + 0x44, 0x14000000);
	} else {
		// disable auto PD/SR
		mmio_wr32(0x08004000 + 0x30, 0x00000000);
		// disable auto ctrl_upd
		mmio_wr32(0x08004000 + 0x1a0, 0xC0400018);
		// disable clock gating
		mmio_wr32(0x0800a000 + 0x14, 0x00000fff);
		// change xpi to single DDR burst
		mmio_wr32(0x08004000 + 0xc, 0x63746371);
		mmio_wr32(0x08004000 + 0x44, 0x14000000);
	}
}

void ctrl_init_update_by_dram_size(uint8_t dram_cap_in_mbyte)
{
	uint8_t dram_cap_in_mbyte_per_dev;

	if (get_ddr_type() == DDR_TYPE_DDR3) {
		rddata = mmio_rd32(0x08004000 + 0x0);
		dram_cap_in_mbyte_per_dev = dram_cap_in_mbyte;
		dram_cap_in_mbyte_per_dev >>= (1 - get_bits_from_value(rddata, 13, 12)); // change sys cap to x16 cap
		dram_cap_in_mbyte_per_dev >>= (2 - get_bits_from_value(rddata, 31, 30)); // change x16 cap to device cap
		switch (dram_cap_in_mbyte_per_dev) {
		case 6:
			mmio_wr32(0x08004000 + 0x64, 0x0071002A);
			mmio_wr32(0x08004000 + 0x120, 0x00000903);
			break;
		case 7:
			mmio_wr32(0x08004000 + 0x64, 0x00710034);
			mmio_wr32(0x08004000 + 0x120, 0x00000903);
			break;
		case 8:
			mmio_wr32(0x08004000 + 0x64, 0x0071004B);
			mmio_wr32(0x08004000 + 0x120, 0x00000904);
			break;
		case 9:
			mmio_wr32(0x08004000 + 0x64, 0x0071007A);
			mmio_wr32(0x08004000 + 0x120, 0x00000905);
			break;
		case 10:
			mmio_wr32(0x08004000 + 0x64, 0x007100A4);
			mmio_wr32(0x08004000 + 0x120, 0x00000907);
			break;
		}
		// toggle refresh_update_level
		mmio_wr32(0x08004000 + 0x60, 0x00000002);
		mmio_wr32(0x08004000 + 0x60, 0x00000000);
	} else {
		rddata = mmio_rd32(0x08004000 + 0x0);
		dram_cap_in_mbyte_per_dev = dram_cap_in_mbyte;
		dram_cap_in_mbyte_per_dev >>= (1 - get_bits_from_value(rddata, 13, 12)); // change sys cap to x16 cap
		dram_cap_in_mbyte_per_dev >>= (2 - get_bits_from_value(rddata, 31, 30)); // change x16 cap to device cap
		if (get_ddr_vendor() == DDR_VENDOR_ETRON_512M_DDR2) {
			switch (dram_cap_in_mbyte_per_dev) {
			case 5:
				mmio_wr32(0x08004000 + 0x64, 0x00510019);
				mmio_wr32(0x08004000 + 0x100, 0x0B01160F);
				mmio_wr32(0x08004000 + 0x120, 0x00000502);
				break;
			case 6:
				mmio_wr32(0x08004000 + 0x64, 0x00510023);
				mmio_wr32(0x08004000 + 0x100, 0x0B01160F);
				mmio_wr32(0x08004000 + 0x120, 0x00000503);
				break;
			case 7:
				mmio_wr32(0x08004000 + 0x64, 0x0051002B);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F160F);
				mmio_wr32(0x08004000 + 0x120, 0x00000503);
				break;
			case 8:
				mmio_wr32(0x08004000 + 0x64, 0x00510041);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F160F);
				mmio_wr32(0x08004000 + 0x120, 0x00000504);
				break;
			case 9:
				mmio_wr32(0x08004000 + 0x64, 0x0051006E);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F160F);
				mmio_wr32(0x08004000 + 0x120, 0x00000505);
				break;
			}
		} else {
			switch (dram_cap_in_mbyte_per_dev) {
			case 5:
				mmio_wr32(0x08004000 + 0x64, 0x00510019);
				mmio_wr32(0x08004000 + 0x100, 0x0B011610);
				mmio_wr32(0x08004000 + 0x120, 0x00000502);
				break;
			case 6:
				mmio_wr32(0x08004000 + 0x64, 0x0051002C);
				mmio_wr32(0x08004000 + 0x100, 0x0A011610);
				mmio_wr32(0x08004000 + 0x120, 0x00000503);
				break;
			case 7:
				mmio_wr32(0x08004000 + 0x64, 0x0051002B);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F1610);
				mmio_wr32(0x08004000 + 0x120, 0x00000503);
				break;
			case 8:
				mmio_wr32(0x08004000 + 0x64, 0x00510041);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F1610);
				mmio_wr32(0x08004000 + 0x120, 0x00000504);
				break;
			case 9:
				mmio_wr32(0x08004000 + 0x64, 0x0051006E);
				mmio_wr32(0x08004000 + 0x100, 0x0B0F1610);
				mmio_wr32(0x08004000 + 0x120, 0x00000505);
				break;
			}
		}

		switch (dram_cap_in_mbyte_per_dev) {
		case 5:
			mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x204, 0x003F0606);
			mmio_wr32(0x08004000 + 0x208, 0x00000000);
			mmio_wr32(0x08004000 + 0x20c, 0x1F1F0000);
			mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x214, 0x040F0404);
			mmio_wr32(0x08004000 + 0x218, 0x04040404);
			mmio_wr32(0x08004000 + 0x21c, 0x00000404);
			mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
			mmio_wr32(0x08004000 + 0x224, 0x04040404);
			mmio_wr32(0x08004000 + 0x228, 0x04040404);
			mmio_wr32(0x08004000 + 0x22c, 0x001F1F04);
			break;
		case 6:
			mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x204, 0x003F0707);
			mmio_wr32(0x08004000 + 0x208, 0x00000000);
			mmio_wr32(0x08004000 + 0x20c, 0x1F000000);
			mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x214, 0x050F0505);
			mmio_wr32(0x08004000 + 0x218, 0x05050505);
			mmio_wr32(0x08004000 + 0x21c, 0x00000505);
			mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
			mmio_wr32(0x08004000 + 0x224, 0x05050505);
			mmio_wr32(0x08004000 + 0x228, 0x05050505);
			mmio_wr32(0x08004000 + 0x22c, 0x001F1F05);
			break;
		case 7:
		case 8:
		case 9:
			mmio_wr32(0x08004000 + 0x200, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x204, 0x00070707);
			mmio_wr32(0x08004000 + 0x208, 0x00000000);
			mmio_wr32(0x08004000 + 0x20c, 0x1F000000);
			mmio_wr32(0x08004000 + 0x210, 0x00001F1F);
			mmio_wr32(0x08004000 + 0x214, 0x060F0606);
			mmio_wr32(0x08004000 + 0x218, 0x06060606);
			mmio_wr32(0x08004000 + 0x21c, 0x00000606);
			mmio_wr32(0x08004000 + 0x220, 0x00003F3F);
			mmio_wr32(0x08004000 + 0x224, 0x06060606);
			mmio_wr32(0x08004000 + 0x228, 0x06060606);
			mmio_wr32(0x08004000 + 0x22c, 0x001F1F06);
			break;
		}
		// toggle refresh_update_level
		mmio_wr32(0x08004000 + 0x60, 0x00000002);
		mmio_wr32(0x08004000 + 0x60, 0x00000000);
	}
}
