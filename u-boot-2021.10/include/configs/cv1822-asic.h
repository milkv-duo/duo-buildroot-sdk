/*
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CV1822_ASIC_H
#define __CV1822_ASIC_H
/* partition definitions header which is created by mkcvipart.py */
/* please do not modify header manually */
#include "cvipart.h"
#include "cvi_panels/cvi_panel_diffs.h"

#define CONFIG_ARMV8_SWITCH_TO_EL1

#define CONFIG_REMAKE_ELF

#define CONFIG_SUPPORT_RAW_INITRD

/* Link Definitions */
/* ATF loads u-boot here for BASE_FVP model */
#define CONFIG_SYS_TEXT_BASE		0x83080000
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_SDRAM_BASE + 0x03f00000)

#define CONFIG_SYS_BOOTM_LEN (64 << 20)      /* Increase max gunzip size */

/* default address for bootm command without arguments */
#define CONFIG_SYS_LOAD_ADDR		0x80080000

/* Generic Interrupt Controller Definitions */
#define GICD_BASE			(0x01F01000)
#define GICC_BASE			(0x01F02000)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (9 << 20))

/* For network descriptor, should be enabled when mmu is okay */
#define CONFIG_SYS_NONCACHED_MEMORY	BIT(20)	/* 1 MiB */

/* 16550 Serial Configuration */
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_NS16550_COM1		0x04140000
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_CLK		25000000
/* include/generated/autoconf.h would define CONFIG_BAUDRATE from drivers/serial/Kconfig (default 115200) */

/*#define CONFIG_MENU_SHOW*/

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_PXE

/* Physical Memory Map */
#define PHYS_SDRAM_1		0x80000000
#define PHYS_SDRAM_1_SIZE	CONFIG_SYS_BOOTMAPSZ
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1

#define BM_UPDATE_FW_START_ADDR             (PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)
#define BM_UPDATE_FW_SIZE		            (0x70000000)
#define BM_UPDATE_FW_FILLBUF_SIZE           (1024 * 512)

#define CONFIG_NR_DRAM_BANKS		1

/* Enable memtest */
#define CONFIG_SYS_MEMTEST_START	PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END		(PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)

/* Enable watchdog */
#define CONFIG_HW_WATCHDOG
#define CONFIG_DESIGNWARE_WATCHDOG
#define CONFIG_WATCHDOG_TIMEOUT_MSECS 42000 // options: 1s, 2s, 5s, 10s, 21, 42s, 85s

/* Download related definitions */
#define SD_UPDATE_SRAM_ADDR  0x0e000004
#define USB_UPDATE_SRAM_ADDR 0x0e000004
#define UPGRADE_SRAM_ADDR    0x0e000030
#define HEADER_ADDR 0x80080000
#define USB_UPDATE_MAGIC 0x4D474E31
/*----------------------------------------------------------------------
 * SPI Flash Configuration
 * ---------------------------------------------------------------------
 */

#ifdef CONFIG_SPI_FLASH
	#define CONFIG_CMD_SF		 /* sf read/sf write/sf erase */
	#define CONFIG_SPI_FLASH_CVSFC
	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	#define CONFIG_FLASH_CFI_MTD
	#define CONFIG_SYS_MAX_FLASH_BANKS 1
	#define CONFIG_SPI_FLASH_MTD
	#define CONFIG_MTD
#endif /* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * SPI NAND Flash Configuration
 *----------------------------------------------------------------------
 */

#ifdef CONFIG_NAND_SUPPORT
	/*#define CONFIG_ENV_IS_IN_NAND*/ /* env in nand flash */
	#define CONFIG_CMD_NAND
	#define CONFIG_CMD_SAVEENV
	#define CONFIG_SYS_MAX_NAND_DEVICE	 1
	#define CONFIG_NAND_FLASH_CVSNFC
	#define CONFIG_SYS_MAX_NAND_CHIPS 1
	/*#define CONFIG_SYS_NAND_SELF_INIT*/
	#define CONFIG_MTD
	#define CONFIG_MTD_RAW_NAND
	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	/* For CMD_UBI && CMD_UBIFS */
	#define CONFIG_RBTREE
	#define CONFIG_LZO
	//#define CONFIG_CMD_UBI
	//#define CONFIG_CMD_UBIFS
	//#define CONFIG_MTD_UBI_WL_THRESHOLD 4096
	//#define CONFIG_MTD_UBI_BEB_LIMIT 20
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
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#define CONFIG_ENV_ADDR			PHYS_SDRAM_1
#define CONFIG_FAT_WRITE
#define CONFIG_ENV_OVERWRITE

/* Support eMMC/SD tuning function for SDR104/HS200 Mode */
#define CONFIG_MMC_SUPPORTS_TUNING
/* Support SD UHS Feature SDR104 Mode */
/* #define CONFIG_MMC_UHS_SUPPORT */
#ifdef CONFIG_EMMC_SUPPORT
/* Support eMMC Feature HS200 Mode */
#define CONFIG_MMC_HS200_SUPPORT
#endif

/* #define CONFIG_USB_DWC2 */
/* #define CONFIG_USB_DWC2_REG_ADDR	0x04340000 */
/* Enable below CONFIG for fastboot */

#define CONFIG_CMD_FASTBOOT
#define CONFIG_FASTBOOT
#define CONFIG_FASTBOOT_FLASH

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

#define CONFIG_UDP_FUNCTION_FASTBOOT
#define CONFIG_FASTBOOT_FLASH_MMC_DEV 0
#define CONFIG_FASTBOOT_BUF_ADDR 0x98000000
#define CONFIG_FASTBOOT_BUF_SIZE 0x8000000

#ifdef CONFIG_USE_DEFAULT_ENV
/* The following Settings are chip dependent */
/******************************************************************************/
	#define UIMAG_ADDR	(0x81200000)
	#define MEM_SISZ	__stringify(CONFIG_SYS_BOOTMAPSZ)

	#ifdef CONFIG_BOOTLOGO
		#define LOGO_RESERVED_ADDR "0x81800000"
		#define LOGO_READ_ADDR "0x84080000"
		#define VO_ALIGNMENT "16"
		#define LOGOSIZE "0x80000"
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
	#else
		#define ROOTARGS "rootfstype=squashfs root=" ROOTFS_DEV
	#endif

	/* BOOTARGS */
	#define PARTS  PART_LAYOUT

	/* config uart */
	#define CONSOLEDEV "ttyS0\0"

	/* config loglevel */
	#ifdef RELEASE
		#define OTHERBOOTARGS   "othbootargs=earlycon release loglevel=8\0"
	#else
		#define OTHERBOOTARGS   "othbootargs=earlycon loglevel=8\0"
	#endif

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
		"mem=" MEM_SISZ "\0" \
		"uImage_addr=" __stringify(UIMAG_ADDR) "\0" \
		"mtdparts=" PARTS "\0" \
		"mtdids=" MTDIDS_DEFAULT "\0" \
		"root=" ROOTARGS "\0" \
		OTHERBOOTARGS \
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
			#define LOAD_LOGO ""
		#else
			#define LOAD_LOGO "mmc dev 0;mmc read " LOGO_READ_ADDR " ${MISC_PART_OFFSET} ${MISC_PART_SIZE};"
		#endif
		#define SHOWLOGOCOMMAND LOAD_LOGO CVI_JPEG START_VO START_VL SET_VO_BG
	#else
		#define SHOWLOGOCMD
	#endif

	#define SET_BOOTARGS "setenv bootargs mem=${mem} ${root} ${mtdparts} " \
					"console=$consoledev,$baudrate $othbootargs;"

	#define CONFIG_BOOTCOMMAND	SHOWLOGOCMD "cvi_update || run norboot || run nandboot ||run emmcboot"

	#ifdef CONFIG_NAND_SUPPORT
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
	#else
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

#endif /* __CV1822_ASIC_H */
