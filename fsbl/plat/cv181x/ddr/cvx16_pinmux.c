#include <mmio.h>
#include <reg_soc.h>
#include <ddr_sys.h>
#include <ddr_pkg_info.h>

void cvx16_pinmux(void)
{
	uartlog("%s\n", __func__);
	//		ddr_debug_wr32(0x4E);
	//		ddr_debug_num_write();
	switch (get_ddr_vendor()) {
	case DDR_VENDOR_NY_4G:
		// DDR3_4G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x12141013);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x0C041503);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x06050001);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x08070B02);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0A0F0E09);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x0016110D);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x02136574);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000008);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76512308);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_NY_2G:
		// DDR3_2G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08070D09);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x0605020B);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x14040100);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x15030E0C);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0A0F1213);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00111016);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82135764);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x67513028);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ESMT_1G:
		// DDR3_1G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08070B09);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x05000206);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0C04010D);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x15030A14);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x10111213);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x000F160E);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x31756024);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000008);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x26473518);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000000);
		break;
	case DDR_VENDOR_ESMT_512M_DDR2:
		// N25_DDR2_512
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x0C06080B);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x070D0904);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x00010502);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x110A0E03);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0F141610);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00151312);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x71840532);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000006);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76103425);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000008);
		break;
	case DDR_VENDOR_ESMT_2G:
		// DDR3_2G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x080B0D06);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x09010407);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x1405020C);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x15000E03);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0A0F1213);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00111016);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82135764);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x67513208);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ETRON_1G:
		// ETRON_DDR3_1G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x0B060908);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x02000107);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0C05040D);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x13141503);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x160A1112);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x000F100E);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x28137564);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76158320);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ESMT_N25_1G:
		// ESMT_N25_DDR3_1G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08060B09);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x02040701);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0C00050D);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x13150314);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x10111216);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x000F0A0E);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82135674);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76153280);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ETRON_512M_DDR2:
		// ETRON_DDR2_512
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x070B090C);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x04050608);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0E02030D);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x110A0100);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0F131614);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00151012);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x86014532);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000007);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76012345);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000008);
		break;
	case DDR_VENDOR_NY_N20_1G:
		// NY_N20_DDR3_1G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08060F0B);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x07040109);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x1405020C);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x15030F00);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0A0F1213);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00111016);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82315764);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x67513208);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_UNILC_N25_1G:
		// UnilC_DDR3_1G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08020B07);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x00030905);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x04010006);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x1300150C);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x16111214);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00000A10);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x08137564);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000002);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76513820);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_UNILC_N21_2G:
		// UnilC_DDR3_2G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08070D09);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x0605020B);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x14040100);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x1503000C);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x0A120013);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00111016);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82135764);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x76513208);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ESMT_N21_2G:
		// ESMT_N21_DDR3_2G
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x08070D09);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x0605020B);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x14040100);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x1503000C);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x16001213);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x0011100A);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x82315746);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x67513208);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000004);
		break;
	case DDR_VENDOR_ESMT_N19_4G:
		// ESMT_DDR3_4G_19nm
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x09050D07);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x00080E02);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0C060B01);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x12031104);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x15001310);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x000A1614);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x31756024);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000008);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x45172380);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000006);
		break;
	case DDR_VENDOR_UNILC_N25_512M_DDR2:
		// UniIC_DDR2_512M
		mmio_write_32(0x0000 + PHYD_BASE_ADDR, 0x00040608);
		mmio_write_32(0x0004 + PHYD_BASE_ADDR, 0x02090B0C);
		mmio_write_32(0x0008 + PHYD_BASE_ADDR, 0x0E01070D);
		mmio_write_32(0x000C + PHYD_BASE_ADDR, 0x0A031605);
		mmio_write_32(0x0010 + PHYD_BASE_ADDR, 0x10110F14);
		mmio_write_32(0x0014 + PHYD_BASE_ADDR, 0x00121513);
		mmio_write_32(0x0018 + PHYD_BASE_ADDR, 0x00000000);
		mmio_write_32(0x001C + PHYD_BASE_ADDR, 0x00000100);
		mmio_write_32(0x0020 + PHYD_BASE_ADDR, 0x61437025);
		mmio_write_32(0x0024 + PHYD_BASE_ADDR, 0x00000008);
		mmio_write_32(0x0028 + PHYD_BASE_ADDR, 0x63410725);
		mmio_write_32(0x002C + PHYD_BASE_ADDR, 0x00000008);
		break;
	}

#ifdef ETRON_DDR2_512
	KC_MSG("pin mux X16 mode ETRON_DDR2_512 setting\n");
	//------------------------------
	//  pin mux base on PHYA
	//------------------------------
	//param_phyd_data_byte_swap_slice0    [1:     0]
	//param_phyd_data_byte_swap_slice1    [9:     8]
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte0_dq0_mux    [3:     0]
	//param_phyd_swap_byte0_dq1_mux    [7:     4]
	//param_phyd_swap_byte0_dq2_mux    [11:    8]
	//param_phyd_swap_byte0_dq3_mux    [15:   12]
	//param_phyd_swap_byte0_dq4_mux    [19:   16]
	//param_phyd_swap_byte0_dq5_mux    [23:   20]
	//param_phyd_swap_byte0_dq6_mux    [27:   24]
	//param_phyd_swap_byte0_dq7_mux    [31:   28]
	rddata = 0x86014532;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte0_dm_mux     [3:     0]
	rddata = 0x00000007;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte1_dq0_mux    [3:     0]
	//param_phyd_swap_byte1_dq1_mux    [7:     4]
	//param_phyd_swap_byte1_dq2_mux    [11:    8]
	//param_phyd_swap_byte1_dq3_mux    [15:   12]
	//param_phyd_swap_byte1_dq4_mux    [19:   16]
	//param_phyd_swap_byte1_dq5_mux    [23:   20]
	//param_phyd_swap_byte1_dq6_mux    [27:   24]
	//param_phyd_swap_byte1_dq7_mux    [31:   28]
	rddata = 0x76012345;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte1_dm_mux     [3:     0]
	rddata = 0x00000008;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca0    [4:     0]
	//param_phyd_swap_ca1    [12:    8]
	//param_phyd_swap_ca2    [20:   16]
	//param_phyd_swap_ca3    [28:   24]
	rddata = 0x070B090C;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca4    [4:     0]
	//param_phyd_swap_ca5    [12:    8]
	//param_phyd_swap_ca6    [20:   16]
	//param_phyd_swap_ca7    [28:   24]
	rddata = 0x04050608;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca8    [4:     0]
	//param_phyd_swap_ca9    [12:    8]
	//param_phyd_swap_ca10   [20:   16]
	//param_phyd_swap_ca11   [28:   24]
	rddata = 0x0E02030D;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca12   [4:     0]
	//param_phyd_swap_ca13   [12:    8]
	//param_phyd_swap_ca14   [20:   16]
	//param_phyd_swap_ca15   [28:   24]
	rddata = 0x110A0100;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca16   [4:     0]
	//param_phyd_swap_ca17   [12:    8]
	//param_phyd_swap_ca18   [20:   16]
	//param_phyd_swap_ca19   [28:   24]
	rddata = 0x0F131614;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca20   [4:     0]
	//param_phyd_swap_ca21   [12:    8]
	//param_phyd_swap_ca22   [20:   16]
	rddata = 0x00151012;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_cke0   [0:0]
	//param_phyd_swap_cs0    [4:4]
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

	//pinmux
#ifdef ESMT_N25_DDR3_1G
	KC_MSG("pin mux X16 mode ESMT_N25_DDR3_1G setting\n");
	//------------------------------
	//  pin mux base on PHYA
	//------------------------------
	//param_phyd_data_byte_swap_slice0    [1:     0]
	//param_phyd_data_byte_swap_slice1    [9:     8]
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte0_dq0_mux    [3:     0]
	//param_phyd_swap_byte0_dq1_mux    [7:     4]
	//param_phyd_swap_byte0_dq2_mux    [11:    8]
	//param_phyd_swap_byte0_dq3_mux    [15:   12]
	//param_phyd_swap_byte0_dq4_mux    [19:   16]
	//param_phyd_swap_byte0_dq5_mux    [23:   20]
	//param_phyd_swap_byte0_dq6_mux    [27:   24]
	//param_phyd_swap_byte0_dq7_mux    [31:   28]
	rddata = 0x82135674;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte0_dm_mux     [3:     0]
	rddata = 0x00000000;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte1_dq0_mux    [3:     0]
	//param_phyd_swap_byte1_dq1_mux    [7:     4]
	//param_phyd_swap_byte1_dq2_mux    [11:    8]
	//param_phyd_swap_byte1_dq3_mux    [15:   12]
	//param_phyd_swap_byte1_dq4_mux    [19:   16]
	//param_phyd_swap_byte1_dq5_mux    [23:   20]
	//param_phyd_swap_byte1_dq6_mux    [27:   24]
	//param_phyd_swap_byte1_dq7_mux    [31:   28]
	rddata = 0x76153280;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_byte1_dm_mux     [3:     0]
	rddata = 0x00000004;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca0    [4:     0]
	//param_phyd_swap_ca1    [12:    8]
	//param_phyd_swap_ca2    [20:   16]
	//param_phyd_swap_ca3    [28:   24]
	rddata = 0x08060B09;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca4    [4:     0]
	//param_phyd_swap_ca5    [12:    8]
	//param_phyd_swap_ca6    [20:   16]
	//param_phyd_swap_ca7    [28:   24]
	rddata = 0x02040701;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca8    [4:     0]
	//param_phyd_swap_ca9    [12:    8]
	//param_phyd_swap_ca10   [20:   16]
	//param_phyd_swap_ca11   [28:   24]
	rddata = 0x0C00050D;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca12   [4:     0]
	//param_phyd_swap_ca13   [12:    8]
	//param_phyd_swap_ca14   [20:   16]
	//param_phyd_swap_ca15   [28:   24]
	rddata = 0x13150314;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca16   [4:     0]
	//param_phyd_swap_ca17   [12:    8]
	//param_phyd_swap_ca18   [20:   16]
	//param_phyd_swap_ca19   [28:   24]
	rddata = 0x10111216;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_ca20   [4:     0]
	//param_phyd_swap_ca21   [12:    8]
	//param_phyd_swap_ca22   [20:   16]
	rddata = 0x000F0A0E;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	//param_phyd_swap_cke0   [0:     0]
	//param_phyd_swap_cs0    [4:     4]
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef ESMT_DDR3_2G
	KC_MSG("pin mux X16 mode ESMT_DDR3_2G setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x82135764;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x67513208;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000004;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x080B0D06;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x09010407;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x1405020C;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x15000E03;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0A0F1213;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00111016;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef ETRON_DDR3_1G
	KC_MSG("pin mux X16 mode ETRON_DDR3_1G setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x28137564;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x76158320;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000004;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0B060908;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x02000107;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C05040D;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x13141503;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x160A1112;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x000F100E;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef DDR3_1G
	KC_MSG("pin mux X16 mode DDR3_1G setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x31756024;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000008;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x26473518;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x08070B09;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x05000206;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C04010D;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x15030A14;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x10111213;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x000F160E;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef DDR3_2G
	KC_MSG("pin mux X16 mode DDR3_2G setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x82135764;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x67513028;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000004;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x08070D09;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0605020B;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x14040100;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x15030E0C;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0A0F1213;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00111016;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef DDR3_4G
	KC_MSG("pin mux X16 mode DDR3_4G setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x02136574;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000008;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x76512308;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000004;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x12141013;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C041503;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x06050001;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x08070B02;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0A0F0E09;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0016110D;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef DDR3_DBG
	KC_MSG("pin mux X16 mode DDR3_DBG setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x30587246;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000001;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x26417538;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0002080E;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x04060D01;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x090C030B;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x05071412;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0A151013;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0016110F;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif
#ifdef DDR3_PINMUX
	KC_MSG("pin mux X16 mode DDR3_6mil setting\n");
	rddata = 0x00000001;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x40613578;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000002;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x03582467;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000001;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x020E0D00;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x07090806;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C05010B;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x12141503;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x100A0413;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00160F11;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef DDR2_512
	KC_MSG("pin mux X16 mode DDR2_512 setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x60851243;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000007;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x67012354;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000008;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C06080B;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x090D0204;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x01050700;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x160A0E03;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0F141110;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00151312;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif

#ifdef N25_DDR2_512
	KC_MSG("pin mux X16 mode N25_DDR2_512 setting\n");
	rddata = 0x00000100;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x71840532;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000006;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x76103425;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000008;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C06080B;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x070D0904;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00010502;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x110A0E03;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x0F141610;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00151312;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif
#ifdef DDR2_PINMUX
	KC_MSG("pin mux X16 mode DDR2 setting\n");
	rddata = 0x00000001;
	mmio_write_32(0x001C + PHYD_BASE_ADDR, rddata);
	rddata = 0x40613578;
	mmio_write_32(0x0020 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000002;
	mmio_write_32(0x0024 + PHYD_BASE_ADDR, rddata);
	rddata = 0x03582467;
	mmio_write_32(0x0028 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000001;
	mmio_write_32(0x002C + PHYD_BASE_ADDR, rddata);
	rddata = 0x020E0D00;
	mmio_write_32(0x0000 + PHYD_BASE_ADDR, rddata);
	rddata = 0x07090806;
	mmio_write_32(0x0004 + PHYD_BASE_ADDR, rddata);
	rddata = 0x0C05010B;
	mmio_write_32(0x0008 + PHYD_BASE_ADDR, rddata);
	rddata = 0x12141503;
	mmio_write_32(0x000C + PHYD_BASE_ADDR, rddata);
	rddata = 0x100A0413;
	mmio_write_32(0x0010 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00160F11;
	mmio_write_32(0x0014 + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	mmio_write_32(0x0018 + PHYD_BASE_ADDR, rddata);
	KC_MSG("pin mux setting }\n");
#endif
}
