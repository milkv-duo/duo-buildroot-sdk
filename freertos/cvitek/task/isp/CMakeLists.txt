#file(GLOB _SOURCES "src/${CHIP}/*.c")
file(GLOB _SOURCES "isp/algo/ae/*.c"
					"isp/algo/awb/*.c"
						#"isp/${CHIP}/isp/src/*.c"
					"isp/${CHIP}/isp/src/isp_lut.c"
					"isp/${CHIP}/isp/src/isp_sts_ctrl.c"
					"isp/${CHIP}/isp/src/isp_mw_compat.c"
					"isp/${CHIP}/isp/src/isp_presharpen_ctrl.c"
					"isp/${CHIP}/isp/src/isp_drc_ctrl.c"
					"isp/${CHIP}/isp/src/isp_ccm_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_mailbox.c"
					"isp/${CHIP}/isp/src/isp_mlsc_ctrl.c"
					"isp/${CHIP}/isp/src/isp_dehaze_ctrl.c"
					"isp/${CHIP}/isp/src/isp_blc_ctrl.c"
					"isp/${CHIP}/isp/src/isp_rgbcac_ctrl.c"
					"isp/${CHIP}/isp/src/isp_interpolate.c"
					"isp/${CHIP}/isp/src/isp_feature_ctrl.c"
					"isp/${CHIP}/isp/src/isp_3a.c"
					"isp/${CHIP}/isp/src/isp_crosstalk_ctrl.c"
					"isp/${CHIP}/isp/src/isp_tun_buf_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_rlsc_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_peri_ctrl.c"
					"isp/${CHIP}/isp/src/isp_bnr_ctrl.c"
					"isp/${CHIP}/isp/src/cvi_isp.c"
						#"isp/${CHIP}/isp/src/isp_param_default.c"
					"isp/${CHIP}/isp/src/isp_demosaic_ctrl.c"
					"isp/${CHIP}/isp/src/isp_dpc_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_proc.c"
					"isp/${CHIP}/isp/src/isp_csc_ctrl.c"
					"isp/${CHIP}/isp/src/isp_cnr_ctrl.c"
					"isp/${CHIP}/isp/src/isp_ca2_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_debug.c"
					"isp/${CHIP}/isp/src/isp_sensor.c"
					"isp/${CHIP}/isp/src/isp_ycontrast_ctrl.c"
					"isp/${CHIP}/isp/src/isp_dci_ctrl.c"
					"isp/${CHIP}/isp/src/isp_ldci_ctrl.c"
					"isp/${CHIP}/isp/src/isp_cac_ctrl.c"
					"isp/${CHIP}/isp/src/isp_lcac_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_motion_ctrl.c"
					"isp/${CHIP}/isp/src/isp_wb_ctrl.c"
					"isp/${CHIP}/isp/src/isp_ynr_ctrl.c"
					"isp/${CHIP}/isp/src/isp_tnr_ctrl.c"
					"isp/${CHIP}/isp/src/isp_ca_ctrl.c"
					"isp/${CHIP}/isp/src/isp_mono_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_clut_ctrl.c"
					"isp/${CHIP}/isp/src/isp_gamma_ctrl.c"
						#"isp/${CHIP}/isp/src/isp_dis_ctrl.c"
					"isp/${CHIP}/isp/src/isp_flow_ctrl.c"
					"isp/${CHIP}/isp/src/isp_fswdr_ctrl.c"
					"isp/${CHIP}/isp/src/isp_sharpen_ctrl.c"

					"isp/${CHIP}/isp_algo/src/isp_algo_gamma.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_fswdr.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_csc.c"
						#"isp/${CHIP}/isp_algo/src/isp_algo_clut.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_bnr.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_blc.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_drc.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_dci.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ldci.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_sharpen.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_cnr.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_rgbcac.c"
						#"isp/${CHIP}/isp_algo/src/isp_algo_dpc.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ycontrast.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_demosaic.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_wb.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_presharpen.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ca.c"
						#"isp/${CHIP}/isp_algo/src/pchip.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_crosstalk.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_dehaze.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_mono.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_tnr.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ccm.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_cac.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_lcac.c"
						#"isp/${CHIP}/isp_algo/src/isp_algo_motion.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_utility.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ynr.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_mlsc.c"
						#"isp/${CHIP}/isp_algo/src/dpcm_api.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_debug.c"
						#"isp/${CHIP}/isp_algo/src/isp_algo_dis.c"
					"isp/${CHIP}/isp_algo/src/isp_algo_ca2.c"
					"isp/${CHIP}/isp_algo/src/pchip.c"
					"isp/${CHIP}/isp_algo/src/isp_iir_api.c"

					"src/${RUN_ARCH}/*.c"
					"isp/${CHIP}/isp/src/isp_mgr_buf.c"

					"isp/${CHIP}/rtos_isp/rtos_isp_mgr.c"
)



include_directories(include)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/arch)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/kernel)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/cif)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/vi)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/common)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/rtos_cmdqu)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/driver/fast_image)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/cif)
include_directories(${CMAKE_INSTALL_INC_PREFIX}/hal/config)
include_directories(../comm/include)

include_directories(isp/include/${CHIP}/)
include_directories(isp/${CHIP}/isp/inc/)
include_directories(isp/${CHIP}/isp/src/)
include_directories(isp/${CHIP}/isp_algo/inc/)
include_directories(isp/${CHIP}/rtos_isp/)

# include_directories(tmp_inc/)

if (CONFIG_CHIP_ARCH_cv181x STREQUAL "y" OR CONFIG_CHIP_ARCH_cv180x STREQUAL "y")
add_compile_definitions(ARCH_RTOS_CV181X)
endif()

add_library(isp STATIC ${_SOURCES})
install(TARGETS isp DESTINATION lib)
