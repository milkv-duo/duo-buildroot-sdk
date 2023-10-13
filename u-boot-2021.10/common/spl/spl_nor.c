// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2012 Stefan Roese <sr@denx.de>
 */

#include <common.h>
#include <image.h>
#include <log.h>
#include <spl.h>
#include "mmio.h"

#define REG_BASE                        0x10000000
#define REG_SPI_CTRL                    0x00
#define REG_SPI_CE_CTRL                 0x04
#define REG_SPI_DLY_CTRL                0x08
#define REG_SPI_DMMR                    0x0C
#define REG_SPI_TRAN_CSR                0x10
#define REG_SPI_TRAN_NUM                0x14
#define REG_SPI_FIFO_PORT               0x18
#define REG_SPI_FIFO_PT                 0x20
#define REG_SPI_INT_STS                 0x28
#define REG_SPI_INT_EN                  0x2C
#define REG_SPI_OPT                     0x30

#define BIT_SPI_CTRL_SCK_DIV_MASK       0x7FF
#define BIT_SPI_DLY_CTRL_CET            (3 << 8)
#define BIT_SPI_DLY_CTRL_NEG_SAMPLE     BIT(14)

#ifndef CONFIG_SYS_OS_BASE
#define CONFIG_SYS_OS_BASE		0x300000
#endif

typedef enum {
	Uninitialized,
	Initialized
} spinor_status_e;
static spinor_status_e spinor_status = Uninitialized;

static void cvi_spi_nor_init(void)
{
	uint32_t reg;

	mmio_write_32(REG_BASE + REG_SPI_DMMR, 0);
	reg = mmio_read_32(REG_BASE + REG_SPI_CTRL);
	reg &= ~BIT_SPI_CTRL_SCK_DIV_MASK;
	/* set clock to 75M */
	reg |= 1;
	mmio_write_32(REG_BASE + REG_SPI_CTRL, reg);
	/* negative sample */
	mmio_write_16(REG_BASE + REG_SPI_DLY_CTRL, BIT_SPI_DLY_CTRL_CET | BIT_SPI_DLY_CTRL_NEG_SAMPLE);
	mmio_write_32(REG_SPI_CE_CTRL, 0);

	reg = 0x003BA9;
	reg &= ~(0xf << 16);
	reg |= (6 << 16);
	mmio_write_32(REG_BASE + REG_SPI_TRAN_CSR, reg);

	mmio_write_32(REG_BASE + REG_SPI_DMMR, 1);

}

static ulong spl_nor_load_read(struct spl_load_info *load, ulong sector,
			       ulong count, void *buf)
{
	debug("%s: sector %lx, count %lx, buf %p\n",
	      __func__, sector, count, buf);
	if (spinor_status != Initialized)
		cvi_spi_nor_init();

	memcpy(buf, (void *)sector, count);

	return count;
}

unsigned long __weak spl_nor_get_uboot_base(void)
{
	return 0;//CONFIG_SYS_UBOOT_BASE;	// not use
}

static int spl_nor_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	__maybe_unused const struct image_header *header;
	__maybe_unused struct spl_load_info load;

	/*
	 * Loading of the payload to SDRAM is done with skipping of
	 * the mkimage header in this SPL NOR driver
	 */
	spl_image->flags |= SPL_COPY_PAYLOAD_ONLY;

#ifdef CONFIG_SPL_OS_BOOT
	if (!spl_start_uboot()) {
		/*
		 * Load Linux from its location in NOR flash to its defined
		 * location in SDRAM
		 */
		if (spinor_status != Initialized) {
			cvi_spi_nor_init();
			spinor_status = Initialized;
		}
		header = (const struct image_header *)(REG_BASE + SPL_BOOT_PART_OFFSET);
#ifdef CONFIG_SPL_LOAD_FIT
		if (image_get_magic(header) == FDT_MAGIC) {
			int ret;

			debug("Found FIT\n");
			load.bl_len = 1;
			load.read = spl_nor_load_read;

			ret = spl_load_simple_fit(spl_image, &load,
						  (REG_BASE + SPL_BOOT_PART_OFFSET),
						  (void *)header);

#if defined CONFIG_SYS_SPL_ARGS_ADDR && defined CONFIG_CMD_SPL_NOR_OFS
			memcpy((void *)CONFIG_SYS_SPL_ARGS_ADDR,
			       (void *)CONFIG_CMD_SPL_NOR_OFS,
			       CONFIG_CMD_SPL_WRITE_SIZE);
#endif
			return ret;
		}
		printf("Not Found FIT\n");
#endif
		if (image_get_os(header) == IH_OS_LINUX) {
			/* happy - was a Linux */
			int ret;

			ret = spl_parse_image_header(spl_image, header);
			if (ret)
				return ret;

			memcpy((void *)spl_image->load_addr,
			       (void *)((REG_BASE + SPL_BOOT_PART_OFFSET) +
					sizeof(struct image_header)),
			       spl_image->size);
#ifdef CONFIG_SYS_FDT_BASE
			spl_image->arg = (void *)CONFIG_SYS_FDT_BASE;
#endif

			return 0;
		} else {
			puts("The Expected Linux image was not found.\n"
			     "Please check your NOR configuration.\n"
			     "Trying to start u-boot now...\n");
		}
	}
#endif

	/*
	 * Load real U-Boot from its location in NOR flash to its
	 * defined location in SDRAM
	 */
#ifdef CONFIG_SPL_LOAD_FIT
	header = (const struct image_header *)spl_nor_get_uboot_base();
	if (image_get_magic(header) == FDT_MAGIC) {
		debug("Found FIT format U-Boot\n");
		load.bl_len = 1;
		load.read = spl_nor_load_read;
		return spl_load_simple_fit(spl_image, &load,
					   spl_nor_get_uboot_base(),
					   (void *)header);
	}
#endif
	if (IS_ENABLED(CONFIG_SPL_LOAD_IMX_CONTAINER)) {
		load.bl_len = 1;
		load.read = spl_nor_load_read;
		return spl_load_imx_container(spl_image, &load,
					      spl_nor_get_uboot_base());
	}

	/* Legacy image handling */
	if (IS_ENABLED(CONFIG_SPL_LEGACY_IMAGE_SUPPORT)) {
		load.bl_len = 1;
		load.read = spl_nor_load_read;
		return spl_load_legacy_img(spl_image, &load,
					   spl_nor_get_uboot_base());
	}

	return 0;
}
SPL_LOAD_IMAGE_METHOD("NOR", 0, BOOT_DEVICE_NOR, spl_nor_load_image);
