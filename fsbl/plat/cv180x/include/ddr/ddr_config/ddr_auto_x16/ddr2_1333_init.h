#ifndef __DDR2_PI_PHY_H__
#define __DDR2_PI_PHY_H__

extern uint32_t ddr_data_rate;
// $Module: reg_cv_phy_param_ddr2_1333 $
// $RegisterBank Version: V 1.0.00 $
// $Author: KC TSAI $
// $Date: Fri, 11 Mar 2022 03:31:19 PM $
//

#define  DDR2_PHY_REG_0_DATA  0b00000011000000100000000100000000
	// param_phyd_swap_ca0:[4:0]=0b00000
	// param_phyd_swap_ca1:[12:8]=0b00001
	// param_phyd_swap_ca2:[20:16]=0b00010
	// param_phyd_swap_ca3:[28:24]=0b00011
#define  DDR2_PHY_REG_1_DATA  0b00000111000001100000010100000100
	// param_phyd_swap_ca4:[4:0]=0b00100
	// param_phyd_swap_ca5:[12:8]=0b00101
	// param_phyd_swap_ca6:[20:16]=0b00110
	// param_phyd_swap_ca7:[28:24]=0b00111
#define  DDR2_PHY_REG_2_DATA  0b00001011000010100000100100001000
	// param_phyd_swap_ca8:[4:0]=0b01000
	// param_phyd_swap_ca9:[12:8]=0b01001
	// param_phyd_swap_ca10:[20:16]=0b01010
	// param_phyd_swap_ca11:[28:24]=0b01011
#define  DDR2_PHY_REG_3_DATA  0b00001111000011100000110100001100
	// param_phyd_swap_ca12:[4:0]=0b01100
	// param_phyd_swap_ca13:[12:8]=0b01101
	// param_phyd_swap_ca14:[20:16]=0b01110
	// param_phyd_swap_ca15:[28:24]=0b01111
#define  DDR2_PHY_REG_4_DATA  0b00010011000100100001000100010000
	// param_phyd_swap_ca16:[4:0]=0b10000
	// param_phyd_swap_ca17:[12:8]=0b10001
	// param_phyd_swap_ca18:[20:16]=0b10010
	// param_phyd_swap_ca19:[28:24]=0b10011
#define  DDR2_PHY_REG_5_DATA  0b00000000000101100001010100010100
	// param_phyd_swap_ca20:[4:0]=0b10100
	// param_phyd_swap_ca21:[12:8]=0b10101
	// param_phyd_swap_ca22:[20:16]=0b10110
#define  DDR2_PHY_REG_6_DATA  0b00000000000000000000000000000000
	// param_phyd_swap_cke0:[0:0]=0b0
	// param_phyd_swap_cs0:[4:4]=0b0
#define  DDR2_PHY_REG_7_DATA  0b00000000000000000000000100000000
	// param_phyd_data_byte_swap_slice0:[1:0]=0b00
	// param_phyd_data_byte_swap_slice1:[9:8]=0b01
#define  DDR2_PHY_REG_8_DATA  0b01110110010101000011001000010000
	// param_phyd_swap_byte0_dq0_mux:[3:0]=0b0000
	// param_phyd_swap_byte0_dq1_mux:[7:4]=0b0001
	// param_phyd_swap_byte0_dq2_mux:[11:8]=0b0010
	// param_phyd_swap_byte0_dq3_mux:[15:12]=0b0011
	// param_phyd_swap_byte0_dq4_mux:[19:16]=0b0100
	// param_phyd_swap_byte0_dq5_mux:[23:20]=0b0101
	// param_phyd_swap_byte0_dq6_mux:[27:24]=0b0110
	// param_phyd_swap_byte0_dq7_mux:[31:28]=0b0111
#define  DDR2_PHY_REG_9_DATA  0b00000000000000000000000000001000
	// param_phyd_swap_byte0_dm_mux:[3:0]=0b1000
#define  DDR2_PHY_REG_10_DATA  0b01110110010101000011001000010000
	// param_phyd_swap_byte1_dq0_mux:[3:0]=0b0000
	// param_phyd_swap_byte1_dq1_mux:[7:4]=0b0001
	// param_phyd_swap_byte1_dq2_mux:[11:8]=0b0010
	// param_phyd_swap_byte1_dq3_mux:[15:12]=0b0011
	// param_phyd_swap_byte1_dq4_mux:[19:16]=0b0100
	// param_phyd_swap_byte1_dq5_mux:[23:20]=0b0101
	// param_phyd_swap_byte1_dq6_mux:[27:24]=0b0110
	// param_phyd_swap_byte1_dq7_mux:[31:28]=0b0111
#define  DDR2_PHY_REG_11_DATA  0b00000000000000000000000000001000
	// param_phyd_swap_byte1_dm_mux:[3:0]=0b1000
#define  DDR2_PHY_REG_16_DATA  0b00000000000000000000000000000000
	// param_phyd_dll_rx_sw_mode:[0:0]=0b0
	// param_phyd_dll_rx_start_cal:[1:1]=0b0
	// param_phyd_dll_rx_cntr_mode:[2:2]=0b0
	// param_phyd_dll_rx_hwrst_time:[3:3]=0b0
	// param_phyd_dll_tx_sw_mode:[16:16]=0b0
	// param_phyd_dll_tx_start_cal:[17:17]=0b0
	// param_phyd_dll_tx_cntr_mode:[18:18]=0b0
	// param_phyd_dll_tx_hwrst_time:[19:19]=0b0
#define  DDR2_PHY_REG_17_DATA  0b00000000011111110000000000001101
	// param_phyd_dll_slave_delay_en:[0:0]=0b1
	// param_phyd_dll_rw_en:[1:1]=0b0
	// param_phyd_dll_avg_mode:[2:2]=0b1
	// param_phyd_dll_upd_wait:[6:3]=0b0001
	// param_phyd_dll_sw_clr:[7:7]=0b0
	// param_phyd_dll_sw_code_mode:[8:8]=0b0
	// param_phyd_dll_sw_code:[23:16]=0b01111111
#define  DDR2_PHY_REG_18_DATA  0b00000000000000000000000000000000
	// param_phya_reg_tx_clk_tx_dline_code_clkn0:[6:0]=0b0000000
	// param_phya_reg_tx_clk_tx_dline_code_clkp0:[14:8]=0b0000000
#define  DDR2_PHY_REG_19_DATA  0b00000000000000000000000000010000
	// param_phya_reg_sel_ddr4_mode:[0:0]=0b0
	// param_phya_reg_sel_lpddr3_mode:[1:1]=0b0
	// param_phya_reg_sel_lpddr4_mode:[2:2]=0b0
	// param_phya_reg_sel_ddr3_mode:[3:3]=0b0
	// param_phya_reg_sel_ddr2_mode:[4:4]=0b1
#define  DDR2_PHY_REG_20_DATA  0b00000000000000000000000000000100
	// param_phyd_dram_class:[3:0]=0b0100
#define  DDR2_PHY_REG_21_DATA  0b00001100000000000000101100000000
	// param_phyd_wrlvl_start_delay_code:[6:0]=0b0000000
	// param_phyd_wrlvl_start_shift_code:[13:8]=0b001011
	// param_phyd_wrlvl_end_delay_code:[22:16]=0b0000000
	// param_phyd_wrlvl_end_shift_code:[29:24]=0b001100
#define  DDR2_PHY_REG_22_DATA  0b00001001000101100000000001001111
	// param_phyd_wrlvl_capture_cnt:[3:0]=0b1111
	// param_phyd_wrlvl_dly_step:[7:4]=0b0100
	// param_phyd_wrlvl_disable:[11:8]=0b0000
	// param_phyd_wrlvl_resp_wait_cnt:[21:16]=0b010110
	// param_phyd_oenz_lead_cnt:[26:23]=0b0010
	// param_phyd_wrlvl_mode:[27:27]=0b1
#define  DDR2_PHY_REG_23_DATA  0b00000000000000000000000000000000
	// param_phyd_wrlvl_sw:[0:0]=0b0
	// param_phyd_wrlvl_sw_upd_req:[1:1]=0b0
	// param_phyd_wrlvl_sw_resp:[2:2]=0b0
	// param_phyd_wrlvl_data_mask:[23:16]=0b00000000
#define  DDR2_PHY_REG_24_DATA  0b00000100000000000000001101110000
	// param_phyd_pigtlvl_back_step:[7:0]=0b01110000
	// param_phyd_pigtlvl_capture_cnt:[11:8]=0b0011
	// param_phyd_pigtlvl_disable:[19:16]=0b0000
	// param_phyd_pigtlvl_dly_step:[27:24]=0b0100
#define  DDR2_PHY_REG_25_DATA  0b00001110000000000000100101000000
	// param_phyd_pigtlvl_start_delay_code:[6:0]=0b1000000
	// param_phyd_pigtlvl_start_shift_code:[13:8]=0b001001
	// param_phyd_pigtlvl_end_delay_code:[22:16]=0b0000000
	// param_phyd_pigtlvl_end_shift_code:[29:24]=0b001110
#define  DDR2_PHY_REG_26_DATA  0b00000000100000000000000000000000
	// param_phyd_pigtlvl_resp_wait_cnt:[5:0]=0b000000
	// param_phyd_pigtlvl_sw:[8:8]=0b0
	// param_phyd_pigtlvl_sw_resp:[13:12]=0b00
	// param_phyd_pigtlvl_sw_upd_req:[16:16]=0b0
	// param_phyd_rx_en_lead_cnt:[23:20]=0b1000
#define  DDR2_PHY_REG_28_DATA  0b00000000000000000000000100001000
	// param_phyd_rgtrack_threshold:[4:0]=0b01000
	// param_phyd_rgtrack_dly_step:[11:8]=0b0001
	// param_phyd_rgtrack_disable:[19:16]=0b0000
#define  DDR2_PHY_REG_29_DATA  0b00000000000001110010000000000000
	// param_phyd_zqcal_wait_count:[3:0]=0b0000
	// param_phyd_zqcal_cycle_count:[15:8]=0b00100000
	// param_phyd_zqcal_hw_mode:[18:16]=0b111
#define  DDR2_PHY_REG_32_DATA  0b00011111000000001110000000000000
	// param_phyd_pirdlvl_dlie_code_start:[7:0]=0b00000000
	// param_phyd_pirdlvl_dlie_code_end:[15:8]=0b11100000
	// param_phyd_pirdlvl_deskew_start:[22:16]=0b0000000
	// param_phyd_pirdlvl_deskew_end:[30:24]=0b0011111
#define  DDR2_PHY_REG_33_DATA  0b00000001000010010001000000001111
	// param_phyd_pirdlvl_trig_lvl_start:[4:0]=0b01111
	// param_phyd_pirdlvl_trig_lvl_end:[12:8]=0b10000
	// param_phyd_pirdlvl_rdvld_start:[20:16]=0b01001
	// param_phyd_pirdlvl_rdvld_end:[28:24]=0b00001
#define  DDR2_PHY_REG_34_DATA  0b00001010000000010000000100010100
	// param_phyd_pirdlvl_dly_step:[3:0]=0b0100
	// param_phyd_pirdlvl_ds_dly_step:[7:4]=0b0001
	// param_phyd_pirdlvl_vref_step:[11:8]=0b0001
	// param_phyd_pirdlvl_disable:[15:12]=0b0000
	// param_phyd_pirdlvl_resp_wait_cnt:[21:16]=0b000001
	// param_phyd_pirdlvl_vref_wait_cnt:[31:24]=0b00001010
#define  DDR2_PHY_REG_35_DATA  0b10101010101010100000000010001111
	// param_phyd_pirdlvl_rx_prebit_deskew_en:[0:0]=0b1
	// param_phyd_pirdlvl_rx_init_deskew_en:[1:1]=0b1
	// param_phyd_pirdlvl_vref_training_en:[2:2]=0b1
	// param_phyd_pirdlvl_rdvld_training_en:[3:3]=0b1
	// param_phyd_pirdlvl_capture_cnt:[7:4]=0b1000
	// param_phyd_pirdlvl_MR1520_BYTE:[15:8]=0b00000000
	// param_phyd_pirdlvl_MR3240:[31:16]=0b1010101010101010
#define  DDR2_PHY_REG_36_DATA  0b00000000000000000011100000000000
	// param_phyd_pirdlvl_data_mask:[8:0]=0b000000000
	// param_phyd_pirdlvl_sw:[9:9]=0b0
	// param_phyd_pirdlvl_sw_upd_req:[10:10]=0b0
	// param_phyd_pirdlvl_sw_resp:[12:11]=0b11
	// param_phyd_pirdlvl_trig_lvl_dqs_follow_dq:[13:13]=0b1
#define  DDR2_PHY_REG_37_DATA  0b00000000000000000000100000000001
	// param_phyd_pirdlvl_rdvld_offset:[3:0]=0b0001
	// param_phyd_pirdlvl_found_cnt_limite:[15:8]=0b00001000
#define  DDR2_PHY_REG_40_DATA  0b00000111010000000000010101000000
	// param_phyd_piwdqlvl_start_delay_code:[6:0]=0b1000000
	// param_phyd_piwdqlvl_start_shift_code:[13:8]=0b000101
	// param_phyd_piwdqlvl_end_delay_code:[22:16]=0b1000000
	// param_phyd_piwdqlvl_end_shift_code:[29:24]=0b000111
#define  DDR2_PHY_REG_41_DATA  0b00000001010000100000010100000100
	// param_phyd_piwdqlvl_tx_vref_start:[4:0]=0b00100
	// param_phyd_piwdqlvl_tx_vref_end:[12:8]=0b00101
	// param_phyd_piwdqlvl_capture_cnt:[19:16]=0b0010
	// param_phyd_piwdqlvl_dly_step:[23:20]=0b0100
	// param_phyd_piwdqlvl_vref_step:[27:24]=0b0001
	// param_phyd_piwdqlvl_disable:[31:28]=0b0000
#define  DDR2_PHY_REG_42_DATA  0b00000000010101010000000000001010
	// param_phyd_piwdqlvl_vref_wait_cnt:[7:0]=0b00001010
	// param_phyd_piwdqlvl_tx_vref_training_en:[8:8]=0b0
	// param_phyd_piwdqlvl_byte_invert_0:[23:16]=0b01010101
#define  DDR2_PHY_REG_43_DATA  0b00000000010101010011110001011010
	// param_phyd_piwdqlvl_dq_pattern_0:[15:0]=0b0011110001011010
	// param_phyd_piwdqlvl_byte_invert_1:[23:16]=0b01010101
#define  DDR2_PHY_REG_44_DATA  0b00000000101010101010010111000011
	// param_phyd_piwdqlvl_dq_pattern_1:[15:0]=0b1010010111000011
	// param_phyd_piwdqlvl_byte_invert_2:[23:16]=0b10101010
#define  DDR2_PHY_REG_45_DATA  0b00000000101010101111000011110000
	// param_phyd_piwdqlvl_dq_pattern_2:[15:0]=0b1111000011110000
	// param_phyd_piwdqlvl_byte_invert_3:[23:16]=0b10101010
#define  DDR2_PHY_REG_46_DATA  0b00011110000000000000111100001111
	// param_phyd_piwdqlvl_dq_pattern_3:[15:0]=0b0000111100001111
	// param_phyd_piwdqlvl_data_mask:[24:16]=0b000000000
	// param_phyd_piwdqlvl_pattern_sel:[28:25]=0b1111
#define  DDR2_PHY_REG_47_DATA  0b00000000000010000011111000010011
	// param_phyd_piwdqlvl_tdfi_phy_wrdata:[2:0]=0b011
	// param_phyd_piwdqlvl_oenz_lead_cnt:[6:3]=0b0010
	// param_phyd_piwdqlvl_sw:[7:7]=0b0
	// param_phyd_piwdqlvl_sw_upd_req:[8:8]=0b0
	// param_phyd_piwdqlvl_sw_resp:[10:9]=0b11
	// param_phyd_piwdqlvl_sw_result:[11:11]=0b1
	// param_phyd_piwdqlvl_dq_mode:[12:12]=0b1
	// param_phyd_piwdqlvl_dm_mode:[13:13]=0b1
	// param_phyd_piwdqlvl_found_cnt_limite:[23:16]=0b00001000
#define  DDR2_PHY_REG_60_DATA  0b00000000000000000000000000000000
	// param_phyd_patch_revision:[31:0]=0b00000000000000000000000000000000
#define  DDR2_PHY_REG_61_DATA  0b00000000000000110000000000110011
	// param_phyd_ca_shift_gating_en:[0:0]=0b1
	// param_phyd_cs_shift_gating_en:[1:1]=0b1
	// param_phyd_cke_shift_gating_en:[2:2]=0b0
	// param_phyd_resetz_shift_gating_en:[3:3]=0b0
	// param_phyd_tx_byte0_shift_gating_en:[4:4]=0b1
	// param_phyd_tx_byte1_shift_gating_en:[5:5]=0b1
	// param_phyd_rx_byte0_shift_gating_en:[16:16]=0b1
	// param_phyd_rx_byte1_shift_gating_en:[17:17]=0b1
#define  DDR2_PHY_REG_62_DATA  0b00000000001000010000000000101100
	// param_phyd_lb_lfsr_seed0:[8:0]=0b000101100
	// param_phyd_lb_lfsr_seed1:[24:16]=0b000100001
#define  DDR2_PHY_REG_63_DATA  0b00000000001101110000000000010110
	// param_phyd_lb_lfsr_seed2:[8:0]=0b000010110
	// param_phyd_lb_lfsr_seed3:[24:16]=0b000110111
#define  DDR2_PHY_REG_64_DATA  0b00000100000000000000000000000000
	// param_phyd_lb_dq_en:[0:0]=0b0
	// param_phyd_lb_dq_go:[1:1]=0b0
	// param_phyd_lb_sw_en:[2:2]=0b0
	// param_phyd_lb_sw_rx_en:[3:3]=0b0
	// param_phyd_lb_sw_rx_mask:[4:4]=0b0
	// param_phyd_lb_sw_odt_en:[5:5]=0b0
	// param_phyd_lb_sw_ca_clkpattern:[6:6]=0b0
	// param_phyd_lb_sync_len:[31:16]=0b0000010000000000
#define  DDR2_PHY_REG_65_DATA  0b00000000000000000000000000000000
	// param_phyd_lb_sw_dout0:[8:0]=0b000000000
	// param_phyd_lb_sw_dout1:[24:16]=0b000000000
#define  DDR2_PHY_REG_67_DATA  0b00000000000000000000000000000000
	// param_phyd_lb_sw_oenz_dout0:[0:0]=0b0
	// param_phyd_lb_sw_oenz_dout1:[1:1]=0b0
	// param_phyd_lb_sw_dqsn0:[4:4]=0b0
	// param_phyd_lb_sw_dqsn1:[5:5]=0b0
	// param_phyd_lb_sw_dqsp0:[8:8]=0b0
	// param_phyd_lb_sw_dqsp1:[9:9]=0b0
	// param_phyd_lb_sw_oenz_dqs_dout0:[12:12]=0b0
	// param_phyd_lb_sw_oenz_dqs_dout1:[13:13]=0b0
#define  DDR2_PHY_REG_68_DATA  0b00000000000000000000000000000000
	// param_phyd_lb_sw_ca_dout:[22:0]=0b00000000000000000000000
#define  DDR2_PHY_REG_69_DATA  0b00000000000000000000000000000000
	// param_phyd_lb_sw_clkn0_dout:[0:0]=0b0
	// param_phyd_lb_sw_clkp0_dout:[4:4]=0b0
	// param_phyd_lb_sw_cke0_dout:[8:8]=0b0
	// param_phyd_lb_sw_resetz_dout:[12:12]=0b0
	// param_phyd_lb_sw_csb0_dout:[16:16]=0b0
#define  DDR2_PHY_REG_70_DATA  0b00000000000000000000000000000000
	// param_phyd_clkctrl_init_complete:[0:0]=0b0
#define  DDR2_PHY_REG_71_DATA  0b00000000000000000110101000010000
	// param_phyd_reg_resetz_dqs_rd_en:[4:4]=0b1
	// param_phyd_rx_upd_tx_sel:[9:8]=0b10
	// param_phyd_tx_upd_rx_sel:[11:10]=0b10
	// param_phyd_rx_en_ext_win:[15:12]=0b0110
	// param_phyd_fifo_rst_sel:[18:16]=0b000
	// param_phyd_fifo_sw_rst:[20:20]=0b0
#define  DDR2_PHY_REG_72_DATA  0b00000000000000000000000000000000
	// param_phyd_phy_int_ack:[31:0]=0b00000000000000000000000000000000
#define  DDR2_PHY_REG_73_DATA  0b11111111111111111111111011110111
	// param_phyd_phy_int_mask:[31:0]=0b11111111111111111111111011110111
#define  DDR2_PHY_REG_74_DATA  0b00000000000000000000000000011111
	// param_phyd_calvl_rst_n:[0:0]=0b1
	// param_phyd_pigtlvl_rst_n:[1:1]=0b1
	// param_phyd_pirdlvl_rst_n:[2:2]=0b1
	// param_phyd_piwdqlvl_rst_n:[3:3]=0b1
	// param_phyd_wrlvl_rst_n:[4:4]=0b1
#define  DDR2_PHY_REG_75_DATA  0b00000000000000000000000000000001
	// param_phyd_clk0_pol:[0:0]=0b1
#define  DDR2_PHY_REG_76_DATA  0b00000000000000000000000100000001
	// param_phyd_tx_ca_oenz:[0:0]=0b1
	// param_phyd_tx_ca_clk0_oenz:[8:8]=0b1
#define  DDR2_PHY_REG_77_DATA  0b00000000000000000000000100000000
	// param_phya_reg_en_test:[0:0]=0b0
	// param_phya_reg_tx_ca_test_en:[1:1]=0b0
	// param_phya_reg_tx_ca_en_ca_loop_back:[2:2]=0b0
	// param_phya_reg_tx_sel_4bit_mode_tx:[8:8]=0b1
	// param_phya_reg_tx_gpio_in:[16:16]=0b0
#define  DDR2_PHY_REG_78_DATA  0b00000000000000000000000000010100
	// param_phya_reg_rx_en_ca_train_mode:[0:0]=0b0
	// param_phya_reg_rx_en_pream_train_mode:[1:1]=0b0
	// param_phya_reg_rx_sel_dqs_wo_pream_mode:[2:2]=0b1
	// param_phya_reg_rx_en_rec_offset:[3:3]=0b0
	// param_phya_reg_rx_sel_4bit_mode_rx:[4:4]=0b1
#define  DDR2_PHY_REG_80_DATA  0b00000000000000000000000000000000
	// param_phya_reg_rx_ddrdll_enautok:[0:0]=0b0
	// param_phya_reg_rx_ddrdll_rstb:[1:1]=0b0
	// param_phya_reg_rx_ddrdll_selckout:[5:4]=0b00
	// param_phya_reg_rx_ddrdll_test:[7:6]=0b00
	// param_phya_reg_rx_ddrdll_sel:[15:8]=0b00000000
	// param_phya_reg_tx_ddrdll_enautok:[16:16]=0b0
	// param_phya_reg_tx_ddrdll_rstb:[17:17]=0b0
	// param_phya_reg_tx_ddrdll_selckout:[21:20]=0b00
	// param_phya_reg_tx_ddrdll_test:[23:22]=0b00
	// param_phya_reg_tx_ddrdll_sel:[31:24]=0b00000000
#define  DDR2_PHY_REG_81_DATA  0b00000000000000000000000000000000
	// param_phya_reg_tx_zq_cmp_en:[0:0]=0b0
	// param_phya_reg_tx_zq_cmp_offset_cal_en:[1:1]=0b0
	// param_phya_reg_tx_zq_ph_en:[2:2]=0b0
	// param_phya_reg_tx_zq_pl_en:[3:3]=0b0
	// param_phya_reg_tx_zq_step2_en:[4:4]=0b0
	// param_phya_reg_tx_zq_cmp_offset:[12:8]=0b00000
	// param_phya_reg_tx_zq_sel_vref:[20:16]=0b00000
#define  DDR2_PHY_REG_82_DATA  0b00000000000000000000100000001000
	// param_phya_reg_tx_zq_golden_drvn:[4:0]=0b01000
	// param_phya_reg_tx_zq_golden_drvp:[12:8]=0b01000
	// param_phya_reg_tx_zq_drvn:[20:16]=0b00000
	// param_phya_reg_tx_zq_drvp:[28:24]=0b00000
#define  DDR2_PHY_REG_83_DATA  0b00000000000000000000000000000000
	// param_phya_reg_tx_zq_en_test_aux:[0:0]=0b0
	// param_phya_reg_tx_zq_en_test_mux:[1:1]=0b0
	// param_phya_reg_sel_zq_high_swing:[2:2]=0b0
	// param_phya_reg_zq_sel_test_out0:[7:4]=0b0000
	// param_phya_reg_zq_sel_test_out1:[11:8]=0b0000
	// param_phya_reg_tx_zq_sel_test_ana_in:[15:12]=0b0000
	// param_phya_reg_tx_zq_sel_gpio_in:[17:16]=0b00
#define  DDR2_PHY_REG_84_DATA  0b00000000000000000000000000000101
	// param_phya_reg_tune_damp_r:[3:0]=0b0101
#define  DDR2_PHY_REG_85_DATA  0b00000000000000000000000100000001
	// param_phyd_sel_cke_oenz:[0:0]=0b1
	// param_phyd_tx_dqsn_default_value:[8:8]=0b1
	// param_phyd_tx_dqsp_default_value:[12:12]=0b0
	// param_phyd_ddr4_2t_preamble:[16:16]=0b0
#define  DDR2_PHY_REG_86_DATA  0b00000000000000000000000000000000
	// param_phya_reg_zqcal_done:[0:0]=0b0
#define  DDR2_PHY_REG_87_DATA  0b00000000000000000000000000000000
	// param_phyd_dbg_sel:[7:0]=0b00000000
	// param_phyd_dbg_sel_en:[16:16]=0b0
#define  DDR2_PHY_REG_89_DATA  0b00000000000000000000000000000000
	// param_phyd_reg_dfs_sel:[0:0]=0b0
#define  DDR2_PHY_REG_90_DATA  0b00000000001100110011001100110001
	// param_phyd_ca_sw_dline_en:[0:0]=0b1
	// param_phyd_byte0_wr_sw_dline_en:[4:4]=0b1
	// param_phyd_byte1_wr_sw_dline_en:[5:5]=0b1
	// param_phyd_byte0_wdqs_sw_dline_en:[8:8]=0b1
	// param_phyd_byte1_wdqs_sw_dline_en:[9:9]=0b1
	// param_phyd_byte0_rd_sw_dline_en:[12:12]=0b1
	// param_phyd_byte1_rd_sw_dline_en:[13:13]=0b1
	// param_phyd_byte0_rdg_sw_dline_en:[16:16]=0b1
	// param_phyd_byte1_rdg_sw_dline_en:[17:17]=0b1
	// param_phyd_byte0_rdqs_sw_dline_en:[20:20]=0b1
	// param_phyd_byte1_rdqs_sw_dline_en:[21:21]=0b1
#define  DDR2_PHY_REG_91_DATA  0b00000000000000000000000000000000
	// param_phyd_ca_raw_dline_upd:[0:0]=0b0
	// param_phyd_byte0_wr_raw_dline_upd:[4:4]=0b0
	// param_phyd_byte1_wr_raw_dline_upd:[5:5]=0b0
	// param_phyd_byte0_wdqs_raw_dline_upd:[8:8]=0b0
	// param_phyd_byte1_wdqs_raw_dline_upd:[9:9]=0b0
	// param_phyd_byte0_rd_raw_dline_upd:[12:12]=0b0
	// param_phyd_byte1_rd_raw_dline_upd:[13:13]=0b0
	// param_phyd_byte0_rdg_raw_dline_upd:[16:16]=0b0
	// param_phyd_byte1_rdg_raw_dline_upd:[17:17]=0b0
	// param_phyd_byte0_rdqs_raw_dline_upd:[20:20]=0b0
	// param_phyd_byte1_rdqs_raw_dline_upd:[21:21]=0b0
#define  DDR2_PHY_REG_92_DATA  0b00000000000000000000000000000000
	// param_phyd_sw_dline_upd_req:[0:0]=0b0
#define  DDR2_PHY_REG_93_DATA  0b00000000000000000000000100000000
	// param_phyd_sw_dfi_phyupd_req:[0:0]=0b0
	// param_phyd_sw_dfi_phyupd_req_clr:[4:4]=0b0
	// param_phyd_sw_phyupd_dline:[8:8]=0b1
#define  DDR2_PHY_REG_96_DATA  0b00000000000000100000000000010000
	// param_phyd_dfi_wrlvl_req:[0:0]=0b0
	// param_phyd_dfi_wrlvl_odt_en:[4:4]=0b1
	// param_phyd_dfi_wrlvl_strobe_cnt:[19:16]=0b0010
#define  DDR2_PHY_REG_97_DATA  0b00000000000000000000000000000000
	// param_phyd_dfi_rdglvl_req:[0:0]=0b0
	// param_phyd_dfi_rdglvl_ddr3_mpr:[4:4]=0b0
#define  DDR2_PHY_REG_98_DATA  0b00000000000000000000000000000000
	// param_phyd_dfi_rdlvl_req:[0:0]=0b0
	// param_phyd_dfi_rdlvl_ddr3_mpr:[4:4]=0b0
#define  DDR2_PHY_REG_99_DATA  0b00000000000001010000010000000000
	// param_phyd_dfi_wdqlvl_req:[0:0]=0b0
	// param_phyd_dfi_wdqlvl_bist_data_en:[4:4]=0b0
	// param_phyd_dfi_wdqlvl_vref_train_en:[10:10]=0b1
	// param_phyd_dfi_wdqlvl_vref_wait_cnt:[23:16]=0b00000101
#define  DDR2_PHY_REG_100_DATA  0b00000000000000100001001000001110
	// param_phyd_dfi_wdqlvl_vref_start:[6:0]=0b0001110
	// param_phyd_dfi_wdqlvl_vref_end:[14:8]=0b0010010
	// param_phyd_dfi_wdqlvl_vref_step:[19:16]=0b0010
#define  DDR2_PHY_REG_128_DATA  0b00000000000000000000000000000000
	// param_phya_reg_byte0_test_en:[0:0]=0b0
	// param_phya_reg_tx_byte0_ddr_test:[15:8]=0b00000000
	// param_phya_reg_rx_byte0_sel_test_in0:[19:16]=0b0000
	// param_phya_reg_rx_byte0_sel_test_in1:[23:20]=0b0000
#define  DDR2_PHY_REG_129_DATA  0b00000000000000000000010001000000
	// param_phya_reg_tx_byte0_en_rx_awys_on:[0:0]=0b0
	// param_phya_reg_tx_byte0_sel_en_rx_dly:[5:4]=0b00
	// param_phya_reg_rx_byte0_sel_en_rx_gen_rst:[6:6]=0b1
	// param_phya_reg_byte0_mask_oenz:[8:8]=0b0
	// param_phya_reg_tx_byte0_en_mask:[10:10]=0b1
	// param_phya_reg_rx_byte0_sel_cnt_mode:[13:12]=0b00
	// param_phya_reg_tx_byte0_sel_int_loop_back:[14:14]=0b0
	// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated:[17:16]=0b00
	// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline:[18:18]=0b0
#define  DDR2_PHY_REG_130_DATA  0b00000000000000000000000000000000
	// param_phyd_reg_reserved_byte0:[31:0]=0b00000000000000000000000000000000
#define  DDR2_PHY_REG_136_DATA  0b00000000000000000000000000000000
	// param_phya_reg_byte1_test_en:[0:0]=0b0
	// param_phya_reg_tx_byte1_ddr_test:[15:8]=0b00000000
	// param_phya_reg_rx_byte1_sel_test_in0:[19:16]=0b0000
	// param_phya_reg_rx_byte1_sel_test_in1:[23:20]=0b0000
#define  DDR2_PHY_REG_137_DATA  0b00000000000000000000010001000000
	// param_phya_reg_tx_byte1_en_rx_awys_on:[0:0]=0b0
	// param_phya_reg_tx_byte1_sel_en_rx_dly:[5:4]=0b00
	// param_phya_reg_rx_byte1_sel_en_rx_gen_rst:[6:6]=0b1
	// param_phya_reg_byte1_mask_oenz:[8:8]=0b0
	// param_phya_reg_tx_byte1_en_mask:[10:10]=0b1
	// param_phya_reg_rx_byte1_sel_cnt_mode:[13:12]=0b00
	// param_phya_reg_tx_byte1_sel_int_loop_back:[14:14]=0b0
	// param_phya_reg_rx_byte1_sel_dqs_dly_for_gated:[17:16]=0b00
	// param_phya_reg_tx_byte1_en_extend_oenz_gated_dline:[18:18]=0b0
#define  DDR2_PHY_REG_138_DATA  0b00000000000000000000000000000000
	// param_phyd_reg_reserved_byte1:[31:0]=0b00000000000000000000000000000000
#define  DDR2_PHY_REG_0_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_ca_sel_lpddr4_pmos_ph_ca:[3:3]=0b0
	// f0_param_phya_reg_tx_clk_sel_lpddr4_pmos_ph_clk:[4:4]=0b0
	// f0_param_phya_reg_tx_sel_lpddr4_pmos_ph:[5:5]=0b0
#define  DDR2_PHY_REG_1_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_ca_drvn_de:[1:0]=0b00
	// f0_param_phya_reg_tx_ca_drvp_de:[5:4]=0b00
	// f0_param_phya_reg_tx_clk0_drvn_de:[9:8]=0b00
	// f0_param_phya_reg_tx_clk0_drvp_de:[13:12]=0b00
	// f0_param_phya_reg_tx_csb_drvn_de:[17:16]=0b00
	// f0_param_phya_reg_tx_csb_drvp_de:[21:20]=0b00
	// f0_param_phya_reg_tx_ca_en_tx_de:[24:24]=0b0
	// f0_param_phya_reg_tx_clk0_en_tx_de:[28:28]=0b0
	// f0_param_phya_reg_tx_csb_en_tx_de:[30:30]=0b0
#define  DDR2_PHY_REG_2_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_ca_sel_dly1t_ca:[22:0]=0b00000000000000000000000
#define  DDR2_PHY_REG_3_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_clk_sel_dly1t_clk0:[0:0]=0b0
	// f0_param_phya_reg_tx_ca_sel_dly1t_cke0:[8:8]=0b0
	// f0_param_phya_reg_tx_ca_sel_dly1t_csb0:[16:16]=0b0
#define  DDR2_PHY_REG_4_F0_DATA  0b00000000000100000000000000000000
	// f0_param_phya_reg_tx_vref_en_free_offset:[0:0]=0b0
	// f0_param_phya_reg_tx_vref_en_rangex2:[1:1]=0b0
	// f0_param_phya_reg_tx_vref_sel_lpddr4divby2p5:[2:2]=0b0
	// f0_param_phya_reg_tx_vref_sel_lpddr4divby3:[3:3]=0b0
	// f0_param_phya_reg_tx_vref_offset:[14:8]=0b0000000
	// f0_param_phya_reg_tx_vref_sel:[20:16]=0b10000
#define  DDR2_PHY_REG_5_F0_DATA  0b00000000000100000000000000000000
	// f0_param_phya_reg_tx_vrefca_en_free_offset:[0:0]=0b0
	// f0_param_phya_reg_tx_vrefca_en_rangex2:[1:1]=0b0
	// f0_param_phya_reg_tx_vrefca_offset:[14:8]=0b0000000
	// f0_param_phya_reg_tx_vrefca_sel:[20:16]=0b10000
#define  DDR2_PHY_REG_6_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_tx_byte_dqs_extend:[2:0]=0b000
#define  DDR2_PHY_REG_7_F0_DATA  0b00000000000000000100000001000000
	// f0_param_phya_reg_rx_byte0_odt_reg:[4:0]=0b00000
	// f0_param_phya_reg_rx_byte0_sel_odt_reg_mode:[6:6]=0b1
	// f0_param_phya_reg_rx_byte1_odt_reg:[12:8]=0b00000
	// f0_param_phya_reg_rx_byte1_sel_odt_reg_mode:[14:14]=0b1
#define  DDR2_PHY_REG_64_F0_DATA  0b00000000000001000001000000000001
	// f0_param_phya_reg_rx_byte0_en_lsmode:[0:0]=0b1
	// f0_param_phya_reg_rx_byte0_hystr:[5:4]=0b00
	// f0_param_phya_reg_rx_byte0_sel_dqs_rec_vref_mode:[8:8]=0b0
	// f0_param_phya_reg_rx_byte0_sel_odt_center_tap:[10:10]=0b0
	// f0_param_phya_reg_byte0_en_rec_vol_mode:[12:12]=0b1
	// f0_param_phya_reg_tx_byte0_force_en_lvstl_ph:[14:14]=0b0
	// f0_param_phya_reg_rx_byte0_force_en_lvstl_odt:[16:16]=0b0
	// f0_param_phya_reg_rx_byte0_en_trig_lvl_rangex2:[18:18]=0b1
	// f0_param_phya_reg_rx_byte0_trig_lvl_en_free_offset:[20:20]=0b0
#define  DDR2_PHY_REG_65_F0_DATA  0b00000000000100000000000000000000
	// f0_param_phya_reg_tx_byte0_drvn_de_dq:[1:0]=0b00
	// f0_param_phya_reg_tx_byte0_drvp_de_dq:[5:4]=0b00
	// f0_param_phya_reg_tx_byte0_drvn_de_dqs:[9:8]=0b00
	// f0_param_phya_reg_tx_byte0_drvp_de_dqs:[13:12]=0b00
	// f0_param_phya_reg_tx_byte0_en_tx_de_dq:[16:16]=0b0
	// f0_param_phya_reg_tx_byte0_en_tx_de_dqs:[20:20]=0b1
#define  DDR2_PHY_REG_66_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_byte0_sel_dly1t_dq:[8:0]=0b000000000
	// f0_param_phya_reg_tx_byte0_sel_dly1t_dqs:[12:12]=0b0
	// f0_param_phya_reg_tx_byte0_sel_dly1t_mask_ranka:[16:16]=0b0
#define  DDR2_PHY_REG_67_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_byte0_vref_sel_lpddr4divby2p5:[0:0]=0b0
	// f0_param_phya_reg_tx_byte0_vref_sel_lpddr4divby3:[4:4]=0b0
	// f0_param_phya_reg_tx_byte0_vref_sel_lpddr4x_voh0p5:[8:8]=0b0
	// f0_param_phya_reg_tx_byte0_vref_sel_lpddr4x_voh0p6:[12:12]=0b0
#define  DDR2_PHY_REG_68_F0_DATA  0b00000000000000000000000000000100
	// f0_param_phyd_reg_rx_byte0_resetz_dqs_offset:[3:0]=0b0100
#define  DDR2_PHY_REG_69_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte0_dq0_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte0_dq1_offset:[14:8]=0b0000000
	// f0_param_phyd_reg_byte0_dq2_offset:[22:16]=0b0000000
	// f0_param_phyd_reg_byte0_dq3_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_70_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte0_dq4_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte0_dq5_offset:[14:8]=0b0000000
	// f0_param_phyd_reg_byte0_dq6_offset:[22:16]=0b0000000
	// f0_param_phyd_reg_byte0_dq7_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_71_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte0_dm_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte0_dqsn_offset:[19:16]=0b0000
	// f0_param_phyd_reg_byte0_dqsp_offset:[27:24]=0b0000
#define  DDR2_PHY_REG_72_F0_DATA  0b00000000000000000000000000000011
	// f0_param_phyd_tx_byte0_tx_oenz_extend:[2:0]=0b011
#define  DDR2_PHY_REG_80_F0_DATA  0b00000000000001000001000000000001
	// f0_param_phya_reg_rx_byte1_en_lsmode:[0:0]=0b1
	// f0_param_phya_reg_rx_byte1_hystr:[5:4]=0b00
	// f0_param_phya_reg_rx_byte1_sel_dqs_rec_vref_mode:[8:8]=0b0
	// f0_param_phya_reg_rx_byte1_sel_odt_center_tap:[10:10]=0b0
	// f0_param_phya_reg_byte1_en_rec_vol_mode:[12:12]=0b1
	// f0_param_phya_reg_tx_byte1_force_en_lvstl_ph:[14:14]=0b0
	// f0_param_phya_reg_rx_byte1_force_en_lvstl_odt:[16:16]=0b0
	// f0_param_phya_reg_rx_byte1_en_trig_lvl_rangex2:[18:18]=0b1
	// f0_param_phya_reg_rx_byte1_trig_lvl_en_free_offset:[20:20]=0b0
#define  DDR2_PHY_REG_81_F0_DATA  0b00000000000100000000000000000000
	// f0_param_phya_reg_tx_byte1_drvn_de_dq:[1:0]=0b00
	// f0_param_phya_reg_tx_byte1_drvp_de_dq:[5:4]=0b00
	// f0_param_phya_reg_tx_byte1_drvn_de_dqs:[9:8]=0b00
	// f0_param_phya_reg_tx_byte1_drvp_de_dqs:[13:12]=0b00
	// f0_param_phya_reg_tx_byte1_en_tx_de_dq:[16:16]=0b0
	// f0_param_phya_reg_tx_byte1_en_tx_de_dqs:[20:20]=0b1
#define  DDR2_PHY_REG_82_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_byte1_sel_dly1t_dq:[8:0]=0b000000000
	// f0_param_phya_reg_tx_byte1_sel_dly1t_dqs:[12:12]=0b0
	// f0_param_phya_reg_tx_byte1_sel_dly1t_mask_ranka:[16:16]=0b0
#define  DDR2_PHY_REG_83_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_tx_byte1_vref_sel_lpddr4divby2p5:[0:0]=0b0
	// f0_param_phya_reg_tx_byte1_vref_sel_lpddr4divby3:[4:4]=0b0
	// f0_param_phya_reg_tx_byte1_vref_sel_lpddr4x_voh0p5:[8:8]=0b0
	// f0_param_phya_reg_tx_byte1_vref_sel_lpddr4x_voh0p6:[12:12]=0b0
#define  DDR2_PHY_REG_84_F0_DATA  0b00000000000000000000000000000100
	// f0_param_phyd_reg_rx_byte1_resetz_dqs_offset:[3:0]=0b0100
#define  DDR2_PHY_REG_85_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte1_dq0_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte1_dq1_offset:[14:8]=0b0000000
	// f0_param_phyd_reg_byte1_dq2_offset:[22:16]=0b0000000
	// f0_param_phyd_reg_byte1_dq3_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_86_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte1_dq4_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte1_dq5_offset:[14:8]=0b0000000
	// f0_param_phyd_reg_byte1_dq6_offset:[22:16]=0b0000000
	// f0_param_phyd_reg_byte1_dq7_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_87_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_byte1_dm_offset:[6:0]=0b0000000
	// f0_param_phyd_reg_byte1_dqsn_offset:[19:16]=0b0000
	// f0_param_phyd_reg_byte1_dqsp_offset:[27:24]=0b0000
#define  DDR2_PHY_REG_88_F0_DATA  0b00000000000000000000000000000011
	// f0_param_phyd_tx_byte1_tx_oenz_extend:[2:0]=0b011
#define  DDR2_PHY_REG_320_F0_DATA  0b00000000000000000000010000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_ca0_sw:[6:0]=0b0000000
	// f0_param_phyd_tx_ca0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_331_F0_DATA  0b00000000000000000000010000000000
	// f0_param_phyd_tx_ca22_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_332_F0_DATA  0b00000000000000000000010000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_cke0_sw:[6:0]=0b0000000
	// f0_param_phyd_tx_cke0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_333_F0_DATA  0b00000000000000000000010000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_csb0_sw:[6:0]=0b0000000
	// f0_param_phyd_tx_cs0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_334_F0_DATA  0b00000000000000000000010000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_resetz_sw:[6:0]=0b0000000
	// f0_param_phyd_tx_reset_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_336_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_ca0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_348_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_cke0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_349_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_csb0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_350_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_ca_tx_dline_code_resetz_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_351_F0_DATA  0b00001000000010000000010000000100
	// f0_param_phya_reg_tx_ca_drvn_ca:[4:0]=0b00100
	// f0_param_phya_reg_tx_ca_drvp_ca:[12:8]=0b00100
	// f0_param_phya_reg_tx_ca_drvn_csb:[20:16]=0b01000
	// f0_param_phya_reg_tx_ca_drvp_csb:[28:24]=0b01000
#define  DDR2_PHY_REG_352_F0_DATA  0b00001000000010000000100000001000
	// f0_param_phya_reg_tx_clk_drvn_clkn0:[4:0]=0b01000
	// f0_param_phya_reg_tx_clk_drvp_clkn0:[12:8]=0b01000
	// f0_param_phya_reg_tx_clk_drvn_clkp0:[20:16]=0b01000
	// f0_param_phya_reg_tx_clk_drvp_clkp0:[28:24]=0b01000
#define  DDR2_PHY_REG_384_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq0_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte0_bit0_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq1_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte0_bit1_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_385_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq2_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte0_bit2_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq3_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte0_bit3_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_386_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq4_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte0_bit4_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq5_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte0_bit5_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_387_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq6_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte0_bit6_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq7_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte0_bit7_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_388_F0_DATA  0b00000000000000000000011001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq8_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte0_bit8_data_shift:[13:8]=0b000110
#define  DDR2_PHY_REG_389_F0_DATA  0b00001101000000000000000000000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dqsn_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dqsp_sw:[22:16]=0b0000000
	// f0_param_phyd_tx_byte0_dqs_shift:[29:24]=0b001101
#define  DDR2_PHY_REG_390_F0_DATA  0b00001010000000000000101100000000
	// f0_param_phyd_tx_byte0_oenz_dqs_shift:[13:8]=0b001011
	// f0_param_phyd_tx_byte0_oenz_shift:[29:24]=0b001010
#define  DDR2_PHY_REG_391_F0_DATA  0b00000000000001000000010000000000
	// f0_param_phyd_tx_byte0_oenz_dqs_extend:[11:8]=0b0100
	// f0_param_phyd_tx_byte0_oenz_extend:[19:16]=0b0100
#define  DDR2_PHY_REG_392_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq0_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq1_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_393_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq2_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq3_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_394_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq4_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq5_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_395_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq6_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq7_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_396_F0_DATA  0b00000000000000000000000001000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dq8_raw:[6:0]=0b1000000
#define  DDR2_PHY_REG_397_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dqsn_raw:[6:0]=0b0000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_dqsp_raw:[22:16]=0b0000000
#define  DDR2_PHY_REG_398_F0_DATA  0b00000000000000000000100000001000
	// f0_param_phya_reg_tx_byte0_drvn_dq:[4:0]=0b01000
	// f0_param_phya_reg_tx_byte0_drvp_dq:[12:8]=0b01000
#define  DDR2_PHY_REG_399_F0_DATA  0b00001000000010000000100000001000
	// f0_param_phya_reg_tx_byte0_drvn_dqsn:[4:0]=0b01000
	// f0_param_phya_reg_tx_byte0_drvp_dqsn:[12:8]=0b01000
	// f0_param_phya_reg_tx_byte0_drvn_dqsp:[20:16]=0b01000
	// f0_param_phya_reg_tx_byte0_drvp_dqsp:[28:24]=0b01000
#define  DDR2_PHY_REG_400_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq0_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte1_bit0_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq1_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte1_bit1_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_401_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq2_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte1_bit2_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq3_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte1_bit3_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_402_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq4_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte1_bit4_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq5_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte1_bit5_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_403_F0_DATA  0b00000110010000000000011001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq6_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte1_bit6_data_shift:[13:8]=0b000110
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq7_sw:[22:16]=0b1000000
	// f0_param_phyd_tx_byte1_bit7_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_404_F0_DATA  0b00000000000000000000011001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq8_sw:[6:0]=0b1000000
	// f0_param_phyd_tx_byte1_bit8_data_shift:[13:8]=0b000110
#define  DDR2_PHY_REG_405_F0_DATA  0b00001101000000000000000000000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dqsn_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dqsp_sw:[22:16]=0b0000000
	// f0_param_phyd_tx_byte1_dqs_shift:[29:24]=0b001101
#define  DDR2_PHY_REG_406_F0_DATA  0b00001010000000000000101100000000
	// f0_param_phyd_tx_byte1_oenz_dqs_shift:[13:8]=0b001011
	// f0_param_phyd_tx_byte1_oenz_shift:[29:24]=0b001010
#define  DDR2_PHY_REG_407_F0_DATA  0b00000000000001000000010000000000
	// f0_param_phyd_tx_byte1_oenz_dqs_extend:[11:8]=0b0100
	// f0_param_phyd_tx_byte1_oenz_extend:[19:16]=0b0100
#define  DDR2_PHY_REG_408_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq0_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq1_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_409_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq2_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq3_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_410_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq4_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq5_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_411_F0_DATA  0b00000000010000000000000001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq6_raw:[6:0]=0b1000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq7_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_412_F0_DATA  0b00000000000000000000000001000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dq8_raw:[6:0]=0b1000000
#define  DDR2_PHY_REG_413_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dqsn_raw:[6:0]=0b0000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_dqsp_raw:[22:16]=0b0000000
#define  DDR2_PHY_REG_414_F0_DATA  0b00000000000000000000100000001000
	// f0_param_phya_reg_tx_byte1_drvn_dq:[4:0]=0b01000
	// f0_param_phya_reg_tx_byte1_drvp_dq:[12:8]=0b01000
#define  DDR2_PHY_REG_415_F0_DATA  0b00001000000010000000100000001000
	// f0_param_phya_reg_tx_byte1_drvn_dqsn:[4:0]=0b01000
	// f0_param_phya_reg_tx_byte1_drvp_dqsn:[12:8]=0b01000
	// f0_param_phya_reg_tx_byte1_drvn_dqsp:[20:16]=0b01000
	// f0_param_phya_reg_tx_byte1_drvp_dqsp:[28:24]=0b01000
#define  DDR2_PHY_REG_448_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_rx_byte0_rx_dq0_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq1_deskew_sw:[14:8]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq2_deskew_sw:[22:16]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq3_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_449_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_rx_byte0_rx_dq4_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq5_deskew_sw:[14:8]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq6_deskew_sw:[22:16]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dq7_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_450_F0_DATA  0b00000000010000000100000000000000
	// f0_param_phyd_reg_rx_byte0_rx_dq8_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte0_rx_dqs_dlie_code_neg_ranka_sw:[15:8]=0b01000000
	// f0_param_phyd_reg_rx_byte0_rx_dqs_dlie_code_pos_ranka_sw:[23:16]=0b01000000
#define  DDR2_PHY_REG_451_F0_DATA  0b00000000000000000000100100000000
	// f0_param_phyd_reg_tx_byte0_tx_dline_code_mask_ranka_sw:[6:0]=0b0000000
	// f0_param_phyd_rx_byte0_mask_shift:[13:8]=0b001001
#define  DDR2_PHY_REG_452_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_rx_byte0_en_shift:[13:8]=0b000000
	// f0_param_phyd_rx_byte0_odt_en_shift:[29:24]=0b000000
#define  DDR2_PHY_REG_453_F0_DATA  0b00000000000001110000111000001110
	// f0_param_phyd_rx_byte0_en_extend:[3:0]=0b1110
	// f0_param_phyd_rx_byte0_odt_en_extend:[11:8]=0b1110
	// f0_param_phyd_rx_byte0_rden_to_rdvld:[20:16]=0b00111
#define  DDR2_PHY_REG_454_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_rx_byte0_rx_dq0_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq1_deskew_raw:[12:8]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq2_deskew_raw:[20:16]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq3_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_455_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_rx_byte0_rx_dq4_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq5_deskew_raw:[12:8]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq6_deskew_raw:[20:16]=0b00000
	// f0_param_phya_reg_rx_byte0_rx_dq7_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_456_F0_DATA  0b01000000010000000000000000000000
	// f0_param_phya_reg_rx_byte0_rx_dq8_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_tx_byte0_tx_dline_code_mask_ranka_raw:[14:8]=0b0000000
	// f0_param_phya_reg_rx_byte0_rx_dqs_dlie_code_neg_ranka_raw:[22:16]=0b1000000
	// f0_param_phya_reg_rx_byte0_rx_dqs_dlie_code_pos_ranka_raw:[30:24]=0b1000000
#define  DDR2_PHY_REG_457_F0_DATA  0b00000000000100000000000000010000
	// f0_param_phya_reg_rx_byte0_trig_lvl_dq:[4:0]=0b10000
	// f0_param_phya_reg_rx_byte0_trig_lvl_dq_offset:[14:8]=0b0000000
	// f0_param_phya_reg_rx_byte0_trig_lvl_dqs:[20:16]=0b10000
	// f0_param_phya_reg_rx_byte0_trig_lvl_dqs_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_460_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_rx_byte1_rx_dq0_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq1_deskew_sw:[14:8]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq2_deskew_sw:[22:16]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq3_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_461_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_reg_rx_byte1_rx_dq4_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq5_deskew_sw:[14:8]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq6_deskew_sw:[22:16]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dq7_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_462_F0_DATA  0b00000000010000000100000000000000
	// f0_param_phyd_reg_rx_byte1_rx_dq8_deskew_sw:[6:0]=0b0000000
	// f0_param_phyd_reg_rx_byte1_rx_dqs_dlie_code_neg_ranka_sw:[15:8]=0b01000000
	// f0_param_phyd_reg_rx_byte1_rx_dqs_dlie_code_pos_ranka_sw:[23:16]=0b01000000
#define  DDR2_PHY_REG_463_F0_DATA  0b00000000000000000000100100000000
	// f0_param_phyd_reg_tx_byte1_tx_dline_code_mask_ranka_sw:[6:0]=0b0000000
	// f0_param_phyd_rx_byte1_mask_shift:[13:8]=0b001001
#define  DDR2_PHY_REG_464_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phyd_rx_byte1_en_shift:[13:8]=0b000000
	// f0_param_phyd_rx_byte1_odt_en_shift:[29:24]=0b000000
#define  DDR2_PHY_REG_465_F0_DATA  0b00000000000001110000111000001110
	// f0_param_phyd_rx_byte1_en_extend:[3:0]=0b1110
	// f0_param_phyd_rx_byte1_odt_en_extend:[11:8]=0b1110
	// f0_param_phyd_rx_byte1_rden_to_rdvld:[20:16]=0b00111
#define  DDR2_PHY_REG_466_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_rx_byte1_rx_dq0_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq1_deskew_raw:[12:8]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq2_deskew_raw:[20:16]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq3_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_467_F0_DATA  0b00000000000000000000000000000000
	// f0_param_phya_reg_rx_byte1_rx_dq4_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq5_deskew_raw:[12:8]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq6_deskew_raw:[20:16]=0b00000
	// f0_param_phya_reg_rx_byte1_rx_dq7_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_468_F0_DATA  0b01000000010000000000000000000000
	// f0_param_phya_reg_rx_byte1_rx_dq8_deskew_raw:[4:0]=0b00000
	// f0_param_phya_reg_tx_byte1_tx_dline_code_mask_ranka_raw:[14:8]=0b0000000
	// f0_param_phya_reg_rx_byte1_rx_dqs_dlie_code_neg_ranka_raw:[22:16]=0b1000000
	// f0_param_phya_reg_rx_byte1_rx_dqs_dlie_code_pos_ranka_raw:[30:24]=0b1000000
#define  DDR2_PHY_REG_469_F0_DATA  0b00000000000100000000000000010000
	// f0_param_phya_reg_rx_byte1_trig_lvl_dq:[4:0]=0b10000
	// f0_param_phya_reg_rx_byte1_trig_lvl_dq_offset:[14:8]=0b0000000
	// f0_param_phya_reg_rx_byte1_trig_lvl_dqs:[20:16]=0b10000
	// f0_param_phya_reg_rx_byte1_trig_lvl_dqs_offset:[30:24]=0b0000000
#ifdef F1_TEST
#define  DDR2_PHY_REG_0_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_ca_sel_lpddr4_pmos_ph_ca:[3:3]=0b0
	// f1_param_phya_reg_tx_clk_sel_lpddr4_pmos_ph_clk:[4:4]=0b0
	// f1_param_phya_reg_tx_sel_lpddr4_pmos_ph:[5:5]=0b0
#define  DDR2_PHY_REG_1_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_ca_drvn_de:[1:0]=0b00
	// f1_param_phya_reg_tx_ca_drvp_de:[5:4]=0b00
	// f1_param_phya_reg_tx_clk0_drvn_de:[9:8]=0b00
	// f1_param_phya_reg_tx_clk0_drvp_de:[13:12]=0b00
	// f1_param_phya_reg_tx_csb_drvn_de:[17:16]=0b00
	// f1_param_phya_reg_tx_csb_drvp_de:[21:20]=0b00
	// f1_param_phya_reg_tx_ca_en_tx_de:[24:24]=0b0
	// f1_param_phya_reg_tx_clk0_en_tx_de:[28:28]=0b0
	// f1_param_phya_reg_tx_csb_en_tx_de:[30:30]=0b0
#define  DDR2_PHY_REG_2_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_ca_sel_dly1t_ca:[22:0]=0b00000000000000000000000
#define  DDR2_PHY_REG_3_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_clk_sel_dly1t_clk0:[0:0]=0b0
	// f1_param_phya_reg_tx_ca_sel_dly1t_cke0:[8:8]=0b0
	// f1_param_phya_reg_tx_ca_sel_dly1t_csb0:[16:16]=0b0
#define  DDR2_PHY_REG_4_F1_DATA  0b00000000000100000000000000000000
	// f1_param_phya_reg_tx_vref_en_free_offset:[0:0]=0b0
	// f1_param_phya_reg_tx_vref_en_rangex2:[1:1]=0b0
	// f1_param_phya_reg_tx_vref_sel_lpddr4divby2p5:[2:2]=0b0
	// f1_param_phya_reg_tx_vref_sel_lpddr4divby3:[3:3]=0b0
	// f1_param_phya_reg_tx_vref_offset:[14:8]=0b0000000
	// f1_param_phya_reg_tx_vref_sel:[20:16]=0b10000
#define  DDR2_PHY_REG_5_F1_DATA  0b00000000000100000000000000000000
	// f1_param_phya_reg_tx_vrefca_en_free_offset:[0:0]=0b0
	// f1_param_phya_reg_tx_vrefca_en_rangex2:[1:1]=0b0
	// f1_param_phya_reg_tx_vrefca_offset:[14:8]=0b0000000
	// f1_param_phya_reg_tx_vrefca_sel:[20:16]=0b10000
#define  DDR2_PHY_REG_6_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_tx_byte_dqs_extend:[2:0]=0b000
#define  DDR2_PHY_REG_7_F1_DATA  0b00000000000000000100000001000000
	// f1_param_phya_reg_rx_byte0_odt_reg:[4:0]=0b00000
	// f1_param_phya_reg_rx_byte0_sel_odt_reg_mode:[6:6]=0b1
	// f1_param_phya_reg_rx_byte1_odt_reg:[12:8]=0b00000
	// f1_param_phya_reg_rx_byte1_sel_odt_reg_mode:[14:14]=0b1
#define  DDR2_PHY_REG_64_F1_DATA  0b00000000000000000001000000000001
	// f1_param_phya_reg_rx_byte0_en_lsmode:[0:0]=0b1
	// f1_param_phya_reg_rx_byte0_hystr:[5:4]=0b00
	// f1_param_phya_reg_rx_byte0_sel_dqs_rec_vref_mode:[8:8]=0b0
	// f1_param_phya_reg_rx_byte0_sel_odt_center_tap:[10:10]=0b0
	// f1_param_phya_reg_byte0_en_rec_vol_mode:[12:12]=0b1
	// f1_param_phya_reg_tx_byte0_force_en_lvstl_ph:[14:14]=0b0
	// f1_param_phya_reg_rx_byte0_force_en_lvstl_odt:[16:16]=0b0
	// f1_param_phya_reg_rx_byte0_en_trig_lvl_rangex2:[18:18]=0b0
	// f1_param_phya_reg_rx_byte0_trig_lvl_en_free_offset:[20:20]=0b0
#define  DDR2_PHY_REG_65_F1_DATA  0b00000000000100000000000000000000
	// f1_param_phya_reg_tx_byte0_drvn_de_dq:[1:0]=0b00
	// f1_param_phya_reg_tx_byte0_drvp_de_dq:[5:4]=0b00
	// f1_param_phya_reg_tx_byte0_drvn_de_dqs:[9:8]=0b00
	// f1_param_phya_reg_tx_byte0_drvp_de_dqs:[13:12]=0b00
	// f1_param_phya_reg_tx_byte0_en_tx_de_dq:[16:16]=0b0
	// f1_param_phya_reg_tx_byte0_en_tx_de_dqs:[20:20]=0b1
#define  DDR2_PHY_REG_66_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_byte0_sel_dly1t_dq:[8:0]=0b000000000
	// f1_param_phya_reg_tx_byte0_sel_dly1t_dqs:[12:12]=0b0
	// f1_param_phya_reg_tx_byte0_sel_dly1t_mask_ranka:[16:16]=0b0
#define  DDR2_PHY_REG_67_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_byte0_vref_sel_lpddr4divby2p5:[0:0]=0b0
	// f1_param_phya_reg_tx_byte0_vref_sel_lpddr4divby3:[4:4]=0b0
	// f1_param_phya_reg_tx_byte0_vref_sel_lpddr4x_voh0p5:[8:8]=0b0
	// f1_param_phya_reg_tx_byte0_vref_sel_lpddr4x_voh0p6:[12:12]=0b0
#define  DDR2_PHY_REG_68_F1_DATA  0b00000000000000000000000000000100
	// f1_param_phyd_reg_rx_byte0_resetz_dqs_offset:[3:0]=0b0100
#define  DDR2_PHY_REG_69_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte0_dq0_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte0_dq1_offset:[14:8]=0b0000000
	// f1_param_phyd_reg_byte0_dq2_offset:[22:16]=0b0000000
	// f1_param_phyd_reg_byte0_dq3_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_70_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte0_dq4_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte0_dq5_offset:[14:8]=0b0000000
	// f1_param_phyd_reg_byte0_dq6_offset:[22:16]=0b0000000
	// f1_param_phyd_reg_byte0_dq7_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_71_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte0_dm_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte0_dqsn_offset:[19:16]=0b0000
	// f1_param_phyd_reg_byte0_dqsp_offset:[27:24]=0b0000
#define  DDR2_PHY_REG_72_F1_DATA  0b00000000000000000000000000000011
	// f1_param_phyd_tx_byte0_tx_oenz_extend:[2:0]=0b011
#define  DDR2_PHY_REG_80_F1_DATA  0b00000000000000000001000000000001
	// f1_param_phya_reg_rx_byte1_en_lsmode:[0:0]=0b1
	// f1_param_phya_reg_rx_byte1_hystr:[5:4]=0b00
	// f1_param_phya_reg_rx_byte1_sel_dqs_rec_vref_mode:[8:8]=0b0
	// f1_param_phya_reg_rx_byte1_sel_odt_center_tap:[10:10]=0b0
	// f1_param_phya_reg_byte1_en_rec_vol_mode:[12:12]=0b1
	// f1_param_phya_reg_tx_byte1_force_en_lvstl_ph:[14:14]=0b0
	// f1_param_phya_reg_rx_byte1_force_en_lvstl_odt:[16:16]=0b0
	// f1_param_phya_reg_rx_byte1_en_trig_lvl_rangex2:[18:18]=0b0
	// f1_param_phya_reg_rx_byte1_trig_lvl_en_free_offset:[20:20]=0b0
#define  DDR2_PHY_REG_81_F1_DATA  0b00000000000100000000000000000000
	// f1_param_phya_reg_tx_byte1_drvn_de_dq:[1:0]=0b00
	// f1_param_phya_reg_tx_byte1_drvp_de_dq:[5:4]=0b00
	// f1_param_phya_reg_tx_byte1_drvn_de_dqs:[9:8]=0b00
	// f1_param_phya_reg_tx_byte1_drvp_de_dqs:[13:12]=0b00
	// f1_param_phya_reg_tx_byte1_en_tx_de_dq:[16:16]=0b0
	// f1_param_phya_reg_tx_byte1_en_tx_de_dqs:[20:20]=0b1
#define  DDR2_PHY_REG_82_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_byte1_sel_dly1t_dq:[8:0]=0b000000000
	// f1_param_phya_reg_tx_byte1_sel_dly1t_dqs:[12:12]=0b0
	// f1_param_phya_reg_tx_byte1_sel_dly1t_mask_ranka:[16:16]=0b0
#define  DDR2_PHY_REG_83_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_tx_byte1_vref_sel_lpddr4divby2p5:[0:0]=0b0
	// f1_param_phya_reg_tx_byte1_vref_sel_lpddr4divby3:[4:4]=0b0
	// f1_param_phya_reg_tx_byte1_vref_sel_lpddr4x_voh0p5:[8:8]=0b0
	// f1_param_phya_reg_tx_byte1_vref_sel_lpddr4x_voh0p6:[12:12]=0b0
#define  DDR2_PHY_REG_84_F1_DATA  0b00000000000000000000000000000100
	// f1_param_phyd_reg_rx_byte1_resetz_dqs_offset:[3:0]=0b0100
#define  DDR2_PHY_REG_85_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte1_dq0_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte1_dq1_offset:[14:8]=0b0000000
	// f1_param_phyd_reg_byte1_dq2_offset:[22:16]=0b0000000
	// f1_param_phyd_reg_byte1_dq3_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_86_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte1_dq4_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte1_dq5_offset:[14:8]=0b0000000
	// f1_param_phyd_reg_byte1_dq6_offset:[22:16]=0b0000000
	// f1_param_phyd_reg_byte1_dq7_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_87_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_byte1_dm_offset:[6:0]=0b0000000
	// f1_param_phyd_reg_byte1_dqsn_offset:[19:16]=0b0000
	// f1_param_phyd_reg_byte1_dqsp_offset:[27:24]=0b0000
#define  DDR2_PHY_REG_88_F1_DATA  0b00000000000000000000000000000011
	// f1_param_phyd_tx_byte1_tx_oenz_extend:[2:0]=0b011
#define  DDR2_PHY_REG_320_F1_DATA  0b00000000000000000000010000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_ca0_sw:[6:0]=0b0000000
	// f1_param_phyd_tx_ca0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_331_F1_DATA  0b00000000000000000000010000000000
	// f1_param_phyd_tx_ca22_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_332_F1_DATA  0b00000000000000000000010000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_cke0_sw:[6:0]=0b0000000
	// f1_param_phyd_tx_cke0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_333_F1_DATA  0b00000000000000000000010000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_csb0_sw:[6:0]=0b0000000
	// f1_param_phyd_tx_cs0_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_334_F1_DATA  0b00000000000000000000010000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_resetz_sw:[6:0]=0b0000000
	// f1_param_phyd_tx_reset_shift_sel:[13:8]=0b000100
#define  DDR2_PHY_REG_336_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_ca0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_348_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_cke0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_349_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_csb0_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_350_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_ca_tx_dline_code_resetz_raw:[6:0]=0b0000000
#define  DDR2_PHY_REG_351_F1_DATA  0b00001000000010000000010000000100
	// f1_param_phya_reg_tx_ca_drvn_ca:[4:0]=0b00100
	// f1_param_phya_reg_tx_ca_drvp_ca:[12:8]=0b00100
	// f1_param_phya_reg_tx_ca_drvn_csb:[20:16]=0b01000
	// f1_param_phya_reg_tx_ca_drvp_csb:[28:24]=0b01000
#define  DDR2_PHY_REG_352_F1_DATA  0b00001000000010000000100000001000
	// f1_param_phya_reg_tx_clk_drvn_clkn0:[4:0]=0b01000
	// f1_param_phya_reg_tx_clk_drvp_clkn0:[12:8]=0b01000
	// f1_param_phya_reg_tx_clk_drvn_clkp0:[20:16]=0b01000
	// f1_param_phya_reg_tx_clk_drvp_clkp0:[28:24]=0b01000
#define  DDR2_PHY_REG_384_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq0_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte0_bit0_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq1_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte0_bit1_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_385_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq2_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte0_bit2_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq3_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte0_bit3_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_386_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq4_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte0_bit4_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq5_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte0_bit5_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_387_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq6_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte0_bit6_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq7_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte0_bit7_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_388_F1_DATA  0b00000000000000000000011001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq8_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte0_bit8_data_shift:[13:8]=0b000110
#define  DDR2_PHY_REG_389_F1_DATA  0b00001101000000000000000000000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dqsn_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dqsp_sw:[22:16]=0b0000000
	// f1_param_phyd_tx_byte0_dqs_shift:[29:24]=0b001101
#define  DDR2_PHY_REG_390_F1_DATA  0b00001010000000000000101100000000
	// f1_param_phyd_tx_byte0_oenz_dqs_shift:[13:8]=0b001011
	// f1_param_phyd_tx_byte0_oenz_shift:[29:24]=0b001010
#define  DDR2_PHY_REG_391_F1_DATA  0b00000000000001000000010000000000
	// f1_param_phyd_tx_byte0_oenz_dqs_extend:[11:8]=0b0100
	// f1_param_phyd_tx_byte0_oenz_extend:[19:16]=0b0100
#define  DDR2_PHY_REG_392_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq0_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq1_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_393_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq2_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq3_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_394_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq4_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq5_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_395_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq6_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq7_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_396_F1_DATA  0b00000000000000000000000001000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dq8_raw:[6:0]=0b1000000
#define  DDR2_PHY_REG_397_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dqsn_raw:[6:0]=0b0000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_dqsp_raw:[22:16]=0b0000000
#define  DDR2_PHY_REG_398_F1_DATA  0b00000000000000000000100000001000
	// f1_param_phya_reg_tx_byte0_drvn_dq:[4:0]=0b01000
	// f1_param_phya_reg_tx_byte0_drvp_dq:[12:8]=0b01000
#define  DDR2_PHY_REG_399_F1_DATA  0b00001000000010000000100000001000
	// f1_param_phya_reg_tx_byte0_drvn_dqsn:[4:0]=0b01000
	// f1_param_phya_reg_tx_byte0_drvp_dqsn:[12:8]=0b01000
	// f1_param_phya_reg_tx_byte0_drvn_dqsp:[20:16]=0b01000
	// f1_param_phya_reg_tx_byte0_drvp_dqsp:[28:24]=0b01000
#define  DDR2_PHY_REG_400_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq0_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte1_bit0_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq1_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte1_bit1_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_401_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq2_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte1_bit2_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq3_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte1_bit3_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_402_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq4_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte1_bit4_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq5_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte1_bit5_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_403_F1_DATA  0b00000110010000000000011001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq6_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte1_bit6_data_shift:[13:8]=0b000110
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq7_sw:[22:16]=0b1000000
	// f1_param_phyd_tx_byte1_bit7_data_shift:[29:24]=0b000110
#define  DDR2_PHY_REG_404_F1_DATA  0b00000000000000000000011001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq8_sw:[6:0]=0b1000000
	// f1_param_phyd_tx_byte1_bit8_data_shift:[13:8]=0b000110
#define  DDR2_PHY_REG_405_F1_DATA  0b00001101000000000000000000000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dqsn_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dqsp_sw:[22:16]=0b0000000
	// f1_param_phyd_tx_byte1_dqs_shift:[29:24]=0b001101
#define  DDR2_PHY_REG_406_F1_DATA  0b00001010000000000000101100000000
	// f1_param_phyd_tx_byte1_oenz_dqs_shift:[13:8]=0b001011
	// f1_param_phyd_tx_byte1_oenz_shift:[29:24]=0b001010
#define  DDR2_PHY_REG_407_F1_DATA  0b00000000000001000000010000000000
	// f1_param_phyd_tx_byte1_oenz_dqs_extend:[11:8]=0b0100
	// f1_param_phyd_tx_byte1_oenz_extend:[19:16]=0b0100
#define  DDR2_PHY_REG_408_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq0_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq1_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_409_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq2_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq3_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_410_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq4_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq5_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_411_F1_DATA  0b00000000010000000000000001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq6_raw:[6:0]=0b1000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq7_raw:[22:16]=0b1000000
#define  DDR2_PHY_REG_412_F1_DATA  0b00000000000000000000000001000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dq8_raw:[6:0]=0b1000000
#define  DDR2_PHY_REG_413_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dqsn_raw:[6:0]=0b0000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_dqsp_raw:[22:16]=0b0000000
#define  DDR2_PHY_REG_414_F1_DATA  0b00000000000000000000100000001000
	// f1_param_phya_reg_tx_byte1_drvn_dq:[4:0]=0b01000
	// f1_param_phya_reg_tx_byte1_drvp_dq:[12:8]=0b01000
#define  DDR2_PHY_REG_415_F1_DATA  0b00001000000010000000100000001000
	// f1_param_phya_reg_tx_byte1_drvn_dqsn:[4:0]=0b01000
	// f1_param_phya_reg_tx_byte1_drvp_dqsn:[12:8]=0b01000
	// f1_param_phya_reg_tx_byte1_drvn_dqsp:[20:16]=0b01000
	// f1_param_phya_reg_tx_byte1_drvp_dqsp:[28:24]=0b01000
#define  DDR2_PHY_REG_448_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_rx_byte0_rx_dq0_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq1_deskew_sw:[14:8]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq2_deskew_sw:[22:16]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq3_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_449_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_rx_byte0_rx_dq4_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq5_deskew_sw:[14:8]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq6_deskew_sw:[22:16]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dq7_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_450_F1_DATA  0b00000000010000000100000000000000
	// f1_param_phyd_reg_rx_byte0_rx_dq8_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte0_rx_dqs_dlie_code_neg_ranka_sw:[15:8]=0b01000000
	// f1_param_phyd_reg_rx_byte0_rx_dqs_dlie_code_pos_ranka_sw:[23:16]=0b01000000
#define  DDR2_PHY_REG_451_F1_DATA  0b00000000000000000000100100000000
	// f1_param_phyd_reg_tx_byte0_tx_dline_code_mask_ranka_sw:[6:0]=0b0000000
	// f1_param_phyd_rx_byte0_mask_shift:[13:8]=0b001001
#define  DDR2_PHY_REG_452_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_rx_byte0_en_shift:[13:8]=0b000000
	// f1_param_phyd_rx_byte0_odt_en_shift:[29:24]=0b000000
#define  DDR2_PHY_REG_453_F1_DATA  0b00000000000001110000111000001110
	// f1_param_phyd_rx_byte0_en_extend:[3:0]=0b1110
	// f1_param_phyd_rx_byte0_odt_en_extend:[11:8]=0b1110
	// f1_param_phyd_rx_byte0_rden_to_rdvld:[20:16]=0b00111
#define  DDR2_PHY_REG_454_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_rx_byte0_rx_dq0_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq1_deskew_raw:[12:8]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq2_deskew_raw:[20:16]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq3_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_455_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_rx_byte0_rx_dq4_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq5_deskew_raw:[12:8]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq6_deskew_raw:[20:16]=0b00000
	// f1_param_phya_reg_rx_byte0_rx_dq7_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_456_F1_DATA  0b01000000010000000000000000000000
	// f1_param_phya_reg_rx_byte0_rx_dq8_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_tx_byte0_tx_dline_code_mask_ranka_raw:[14:8]=0b0000000
	// f1_param_phya_reg_rx_byte0_rx_dqs_dlie_code_neg_ranka_raw:[22:16]=0b1000000
	// f1_param_phya_reg_rx_byte0_rx_dqs_dlie_code_pos_ranka_raw:[30:24]=0b1000000
#define  DDR2_PHY_REG_457_F1_DATA  0b00000000000100000000000000010000
	// f1_param_phya_reg_rx_byte0_trig_lvl_dq:[4:0]=0b10000
	// f1_param_phya_reg_rx_byte0_trig_lvl_dq_offset:[14:8]=0b0000000
	// f1_param_phya_reg_rx_byte0_trig_lvl_dqs:[20:16]=0b10000
	// f1_param_phya_reg_rx_byte0_trig_lvl_dqs_offset:[30:24]=0b0000000
#define  DDR2_PHY_REG_460_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_rx_byte1_rx_dq0_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq1_deskew_sw:[14:8]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq2_deskew_sw:[22:16]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq3_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_461_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_reg_rx_byte1_rx_dq4_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq5_deskew_sw:[14:8]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq6_deskew_sw:[22:16]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dq7_deskew_sw:[30:24]=0b0000000
#define  DDR2_PHY_REG_462_F1_DATA  0b00000000010000000100000000000000
	// f1_param_phyd_reg_rx_byte1_rx_dq8_deskew_sw:[6:0]=0b0000000
	// f1_param_phyd_reg_rx_byte1_rx_dqs_dlie_code_neg_ranka_sw:[15:8]=0b01000000
	// f1_param_phyd_reg_rx_byte1_rx_dqs_dlie_code_pos_ranka_sw:[23:16]=0b01000000
#define  DDR2_PHY_REG_463_F1_DATA  0b00000000000000000000100100000000
	// f1_param_phyd_reg_tx_byte1_tx_dline_code_mask_ranka_sw:[6:0]=0b0000000
	// f1_param_phyd_rx_byte1_mask_shift:[13:8]=0b001001
#define  DDR2_PHY_REG_464_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phyd_rx_byte1_en_shift:[13:8]=0b000000
	// f1_param_phyd_rx_byte1_odt_en_shift:[29:24]=0b000000
#define  DDR2_PHY_REG_465_F1_DATA  0b00000000000001110000111000001110
	// f1_param_phyd_rx_byte1_en_extend:[3:0]=0b1110
	// f1_param_phyd_rx_byte1_odt_en_extend:[11:8]=0b1110
	// f1_param_phyd_rx_byte1_rden_to_rdvld:[20:16]=0b00111
#define  DDR2_PHY_REG_466_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_rx_byte1_rx_dq0_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq1_deskew_raw:[12:8]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq2_deskew_raw:[20:16]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq3_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_467_F1_DATA  0b00000000000000000000000000000000
	// f1_param_phya_reg_rx_byte1_rx_dq4_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq5_deskew_raw:[12:8]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq6_deskew_raw:[20:16]=0b00000
	// f1_param_phya_reg_rx_byte1_rx_dq7_deskew_raw:[28:24]=0b00000
#define  DDR2_PHY_REG_468_F1_DATA  0b01000000010000000000000000000000
	// f1_param_phya_reg_rx_byte1_rx_dq8_deskew_raw:[4:0]=0b00000
	// f1_param_phya_reg_tx_byte1_tx_dline_code_mask_ranka_raw:[14:8]=0b0000000
	// f1_param_phya_reg_rx_byte1_rx_dqs_dlie_code_neg_ranka_raw:[22:16]=0b1000000
	// f1_param_phya_reg_rx_byte1_rx_dqs_dlie_code_pos_ranka_raw:[30:24]=0b1000000
#define  DDR2_PHY_REG_469_F1_DATA  0b00000000000100000000000000010000
	// f1_param_phya_reg_rx_byte1_trig_lvl_dq:[4:0]=0b10000
	// f1_param_phya_reg_rx_byte1_trig_lvl_dq_offset:[14:8]=0b0000000
	// f1_param_phya_reg_rx_byte1_trig_lvl_dqs:[20:16]=0b10000
	// f1_param_phya_reg_rx_byte1_trig_lvl_dqs_offset:[30:24]=0b0000000
#endif //F1_TEST
void ddrc_init(void);
void phy_init(void);
void ctrl_init_high_patch(void);
void ctrl_init_low_patch(void);
//void ctrl_init_detect_dram_size(uint8_t * dram_cap_in_mbyte);
void ctrl_init_update_by_dram_size(uint8_t dram_cap_in_mbyte);

#endif /* __DDR_PI_PHY_H__ */
