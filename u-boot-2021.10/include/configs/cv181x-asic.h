/*
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CV181X_ASIC_H__
#define __CV181X_ASIC_H__

#include <../../../board/cvitek/cv181x/cv181x_reg.h>

/* defined in cvipart.h */
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_OFFSET_REDUND
#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#undef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_SECT_SIZE

/* cvi_board_memmap.h is generated from build/boards/{CHIP_ARCH}/{BOARD}/memmap.py */
#include "cvi_board_memmap.h"
/* partition definitions header which is created by mkcvipart.py */
/* please do not modify header manually */
#include "cvipart.h"
#include "cvi_panels/cvi_panel_diffs.h"

// defined in this .h
#undef CONFIG_BOOTCOMMAND

#if defined(__aarch64__)
#define CONFIG_ARMV8_SWITCH_TO_EL1
#endif

#define CONFIG_REMAKE_ELF

/* Physical Memory Map */
#define CONFIG_SYS_RESVIONSZ		CVIMMAP_ION_SIZE
#define CONFIG_SYS_RESVLOGOSZ		CVIMMAP_BOOTLOGO_SIZE
#define CONFIG_SYS_BOOTMAPSZ		CVIMMAP_KERNEL_MEMORY_SIZE

#define PHYS_SDRAM_1			CVIMMAP_KERNEL_MEMORY_ADDR
#define PHYS_SDRAM_1_SIZE		CVIMMAP_KERNEL_MEMORY_SIZE
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* Link Definitions */
#define CONFIG_SYS_INIT_SP_ADDR		CVIMMAP_CONFIG_SYS_INIT_SP_ADDR

/* default address for bootm command without arguments */
#define CONFIG_SYS_LOAD_ADDR		0x80080000
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)

/* Generic Interrupt Controller Definitions */
#define GICD_BASE			(0x01F01000)
#define GICC_BASE			(0x01F02000)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (9 << 20))

#if defined(__aarch64__)
#define CONFIG_SYS_NONCACHED_MEMORY	BIT(20)	/* 1 MiB */
#endif

#if defined(__riscv)
#define CONFIG_SYS_CACHELINE_SIZE	64
#endif

// Frequency of ARM arch timer and RISC-V rdtime
#define SYS_COUNTER_FREQ_IN_SECOND 25000000

/* 16550 Serial Configuration */
#define CONFIG_SYS_NS16550_COM1		0x04140000
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_CLK		25000000

/* include/generated/autoconf.h would define CONFIG_BAUDRATE from drivers/serial/Kconfig (default 115200) */

/*#define CONFIG_MENU_SHOW*/


/* Download related definitions */
#define UPGRADE_SRAM_ADDR    0x0e000030
#define UBOOT_PID_SRAM_ADDR  0x0e000030
#define UPDATE_ADDR	CVIMMAP_ION_ADDR
#define HEADER_ADDR	UPDATE_ADDR
#define USB_UPDATE_MAGIC MAGIC_NUM_USB_DL
/*----------------------------------------------------------------------
 * SPI Flash Configuration
 * ---------------------------------------------------------------------
 */

#ifdef CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_CVSFC

	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	#define CONFIG_FLASH_CFI_MTD
	#define CONFIG_SYS_MAX_FLASH_BANKS 1
	#define CONFIG_SPI_FLASH_MTD
#endif /* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * SPI NAND Flash Configuration
 *----------------------------------------------------------------------
 */

#ifdef CONFIG_NAND_SUPPORT
	/*#define CONFIG_ENV_IS_IN_NAND*/ /* env in nand flash */
	#define CONFIG_CMD_NAND
	#define CONFIG_SYS_MAX_NAND_DEVICE	 1

	#define CONFIG_NAND_FLASH_CVSNFC
	#define CONFIG_SYS_MAX_NAND_CHIPS 1
	/*#define CONFIG_SYS_NAND_SELF_INIT*/

	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	/* For CMD_UBI && CMD_UBIFS */
	#define CONFIG_RBTREE
	#define CONFIG_LZO
	//#define CONFIG_CMD_UBI
	//#define CONFIG_CMD_UBIFS
	#define CONFIG_MTD_UBI_WL_THRESHOLD 4096
	#define CONFIG_MTD_UBI_BEB_LIMIT 20
	#define NANDBOOT_V2
#endif /* CONFIG_NAND_SUPPORT */

#ifdef CONFIG_NAND_FLASH_CVSNFC
	#define SPI_NAND_TX_DATA_BASE 0x4060060
	#define SPI_NAND_RX_DATA_BASE 0x4060064

	#define SPI_NAND_REG_BASE 0x4060000

	#define CONFIG_SYS_NAND_MAX_CHIPS		1
	#define CONFIG_SYS_NAND_BASE			SPI_NAND_REG_BASE
	#define CONFIG_CVSNFC_MAX_CHIP			CONFIG_SYS_MAX_NAND_DEVICE
	#define CONFIG_CVSNFC_REG_BASE_ADDRESS		SPI_NAND_REG_BASE
	#define CONFIG_CVSNFC_BUFFER_BASE_ADDRESS	SPI_NAND_MEM_BASE
	#define CONFIG_CVSNFC_HARDWARE_PAGESIZE_ECC
	#define CONFIG_SYS_NAND_BASE_LIST		{CONFIG_SYS_NAND_BASE}

#endif /* CONFIG_NAND_SUPPORT */

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#ifndef CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_SECT_SIZE		0x00040000
#endif

#define CONFIG_ENV_OVERWRITE

#define CONFIG_MMC_UHS_SUPPORT
#define CONFIG_MMC_HS200_SUPPORT
#define CONFIG_MMC_SUPPORTS_TUNING
/* #define CONFIG_USB_DWC2 */
/* #define CONFIG_USB_DWC2_REG_ADDR	0x04340000 */
/* Enable below CONFIG for fastboot */

/* To use usb fastboot, you need to enable below Kconfig
 * CONFIG_USB, CONFIG_USB_GADGET and CONFIG_USB_GADGET_DWC2_OTG
 */

#ifdef CONFIG_USB_GADGET
#define CONFIG_USB_FUNCTION_FASTBOOT
#define CONFIG_USB_GADGET_DOWNLOAD
#define CONFIG_G_DNL_MANUFACTURER "Cvitek"
#define CONFIG_G_DNL_VENDOR_NUM   0x18d1
#define CONFIG_G_DNL_PRODUCT_NUM 0x4ee0
#endif

#define CONFIG_IPADDR			192.168.0.3
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_GATEWAYIP		192.168.0.11
#define CONFIG_SERVERIP			192.168.56.101

#ifdef CONFIG_USE_DEFAULT_ENV
/* The following Settings are chip dependent */
/******************************************************************************/
	#define UIMAG_ADDR	CVIMMAP_UIMAG_ADDR

	#ifdef CONFIG_BOOTLOGO
		#define LOGO_RESERVED_ADDR __stringify(CVIMMAP_BOOTLOGO_ADDR)//yuv load addr
		#define LOGO_READ_ADDR "0x84080000" //jpeg load addr
		#define VO_ALIGNMENT "16"
		#define LOGOSIZE "0x80000" //jpeg max size
	#endif
/******************************************************************************/
/* define common env */
/*******************************************************************************/
	/* Config FDT_NO */
	#ifndef USE_HOSTCC
		#define FDT_NO __stringify(CVICHIP) "_" __stringify(CVIBOARD)
	#else
		#define FDT_NO ""
	#endif

	/* config root */
	#ifdef CONFIG_NAND_SUPPORT
		#ifdef CONFIG_SKIP_RAMDISK
			#define ROOTARGS "ubi.mtd=ROOTFS ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs"

		#else
			#define ROOTARGS "ubi.mtd=ROOTFS ubi.block=0,0"
		#endif /* CONFIG_SKIP_RAMDISK */
	#elif defined(CONFIG_SD_BOOT) || defined(CONFIG_EMMC_SUPPORT)
		#define ROOTARGS "root=" ROOTFS_DEV " rootwait rw"
	#else
		#define ROOTARGS "rootfstype=squashfs rootwait ro root=" ROOTFS_DEV
	#endif

	/* BOOTARGS */
	#define PARTS  PART_LAYOUT

	/* config uart */
	#define CONSOLEDEV "ttyS0\0"

	/* config loglevel */
	#ifdef RELEASE
		#define CONSOLE_LOGLEVEL   " loglevel=0"
		#define EARLYCON_RELEASE   " release "
	#else
		#define CONSOLE_LOGLEVEL   " loglevel=9"
		#define EARLYCON_RELEASE   " "
	#endif

	#define OTHERBOOTARGS   "earlycon=sbi riscv.fwsz="  __stringify(CVIMMAP_OPENSBI_SIZE) " " \
		EARLYCON_RELEASE CONSOLE_LOGLEVEL

	/* config mtdids */
	#ifdef CONFIG_NAND_SUPPORT
		#define MTDIDS_DEFAULT "nand0=cvsnfc"
	#elif CONFIG_SPI_FLASH
		#define MTDIDS_DEFAULT "nor1=flash-0"
	#else
		#define MTDIDS_DEFAULT ""
	#endif

	#define CONFIG_EXTRA_ENV_SETTINGS	\
		"netdev=eth0\0"		\
		"consoledev=" CONSOLEDEV  \
		"baudrate=115200\0" \
		"uImage_addr=" __stringify(UIMAG_ADDR) "\0" \
		"update_addr=" __stringify(UPDATE_ADDR) "\0" \
		"mtdparts=" PARTS "\0" \
		"mtdids=" MTDIDS_DEFAULT "\0" \
		"root=" ROOTARGS "\0" \
		"sdboot=" SD_BOOTM_COMMAND "\0" \
		"othbootargs=" OTHERBOOTARGS "\0" \
		PARTS_OFFSET

/********************************************************************************/
	/* UBOOT_VBOOT commands */
	#ifdef UBOOT_VBOOT
		#define UBOOT_VBOOT_BOOTM_COMMAND \
					"aes_itb dec_fdt_key 0 ${uImage_addr} ${uImage_addr}; " \
					"if test $? -ne 0; then " \
					"  echo ITB decryption failed; " \
					"else; " \
					"  bootm ${uImage_addr}#config-" FDT_NO ";" \
					"fi;"
	#else
		#define UBOOT_VBOOT_BOOTM_COMMAND "bootm ${uImage_addr}#config-" FDT_NO ";"
	#endif

	/* BOOTLOGO */
	#ifdef CONFIG_BOOTLOGO
		#define SHOWLOGOCMD "run showlogo;"

		#ifdef CONFIG_NAND_SUPPORT
			#define LOAD_LOGO "nand read " LOGO_READ_ADDR " MISC;"
		#elif defined(CONFIG_SPI_FLASH)
			#define LOAD_LOGO "sf probe;sf read " LOGO_READ_ADDR " ${MISC_PART_OFFSET} ${MISC_PART_SIZE};"
		#else
			#define LOAD_LOGO "mmc dev 0;mmc read " LOGO_READ_ADDR " ${MISC_PART_OFFSET} ${MISC_PART_SIZE};"
		#endif
		#define SHOWLOGOCOMMAND LOAD_LOGO CVI_JPEG START_VO START_VL SET_VO_BG
	#else
		#define SHOWLOGOCMD
	#endif

	#define SET_BOOTARGS "setenv bootargs ${reserved_mem} ${root} ${mtdparts} " \
					"console=$consoledev,$baudrate $othbootargs;"

	#define SD_BOOTM_COMMAND \
				SET_BOOTARGS \
				"echo Boot from SD ...;" \
				"mmc dev 0 && fatload mmc 0 ${uImage_addr} boot.sd; " \
				"if test $? -eq 0; then " \
				UBOOT_VBOOT_BOOTM_COMMAND \
				"fi;"

	#ifndef CONFIG_SD_BOOT
		#ifdef CONFIG_ENABLE_ALIOS_UPDATE
			#define CONFIG_BOOTCOMMAND	"cvi_update_rtos"
		#else
			#define CONFIG_BOOTCOMMAND	SHOWLOGOCMD "cvi_update || run norboot || run nandboot ||run emmcboot"
		#endif
	#else
		#define CONFIG_BOOTCOMMAND	SHOWLOGOCMD "run sdboot"
	#endif

	#if defined(CONFIG_NAND_SUPPORT)
	/* For spi nand boot, need to reset DMA and its setting before exiting uboot */
	/* 0x4330058 : DMA reset */
	/* 0x3000154 : restore DMA remap to 0 */

		#define CONFIG_NANDBOOTCOMMAND \
				SET_BOOTARGS \
				"nand read ${uImage_addr} BOOT;" \
				"mw.l 4330058 1 1; md.l 4330058 1; mw.l 3000154 0 1;" \
				UBOOT_VBOOT_BOOTM_COMMAND
	#elif defined(CONFIG_SPI_FLASH)
		#define CONFIG_NORBOOTCOMMAND \
				SET_BOOTARGS \
				"sf probe;sf read ${uImage_addr} ${BOOT_PART_OFFSET} ${BOOT_PART_SIZE};" \
				UBOOT_VBOOT_BOOTM_COMMAND
	#elif defined(CONFIG_EMMC_SUPPORT)
		#define CONFIG_EMMCBOOTCOMMAND \
				SET_BOOTARGS \
				"mmc dev 0 ;"		\
				"mmc read ${uImage_addr} ${BOOT_PART_OFFSET} ${BOOT_PART_SIZE} ;"		\
				UBOOT_VBOOT_BOOTM_COMMAND
	#endif

#else
	/* define your environment */
	#define CONFIG_BOOTCOMMAND ""

#endif /* CONFIG_USE_DEFAULT_ENV */

#define CVI_SPL_BOOTAGRS \
	PARTS " "  \
	ROOTARGS " " \
	"console=ttyS0,115200 " \
	OTHERBOOTARGS "\0"
#endif /* __CV181X_ASIC_H__ */
