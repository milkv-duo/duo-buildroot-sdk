#ifndef CREATE_SV_H
#define CREATE_SV_H
#include <sys/types.h>

#define SPI_NAND_VECTOR_SIGNATURE DESC_CONV("SPNV")
#define PRODUCTION_FW_SIGNATURE DESC_CONV("PTFM")

#define SPI_NAND_VECTOR_VERSION (0x18802001)

#define SPI_NAND_BASE_DATA_BACKUP_COPY (2)

#define SPI_NAND_FIP_DATA_BLOCK_COUNT (8)
#define SPI_NAND_FIP_DATA_BACKUP_COUNT (2)

#define SPI_NAND_FIP_RSVD_BLOCK_COUNT                                          \
	(4) // first 4 blocks are reserved for sys base vector
#define SPI_NAND_BASE_BLOCK_COUNT (32) // must aligned to 8
#define SPI_NAND_BASE_BLOCK_MAX_COUNT_ENTRY (SPI_NAND_BASE_BLOCK_COUNT / 8)

#define BIT(nr) (1UL << (nr))

#define MAX_SPARE_SIZE 128
#define MAX_BLOCK_CNT 4096
#define NUMBER_PATCH_SET 128

#define _512B (512)
#define _1K (1024)
#define _2K (2048)
#define _4K (4096)
#define _8K (8192)
#define _16K (16384)
#define _32K (32768)
#define _64K (0x10000UL)
#define _128K (0x20000UL)
#define _256K (0x40000UL)
#define _512K (0x80000UL)
#define _1M (0x100000UL)
#define _2M (0x200000UL)
#define _4M (0x400000UL)
#define _8M (0x800000UL)
#define _16M (0x1000000UL)
#define _32M (0x2000000UL)
#define _64M (0x4000000UL)
#define _128M (0x8000000UL)
#define _256M (0x10000000UL)
#define _512M (0x20000000UL)
#define _1G (0x40000000ULL)
#define _2G (0x80000000ULL)
#define _4G (0x100000000ULL)
#define _8G (0x200000000ULL)
#define _16G (0x400000000ULL)
#define _64G (0x1000000000ULL)

#define ECC_TYPE_0BIT 0x0
#define ECC_TYPE_8BIT 0x1
#define ECC_TYPE_16BIT 0x2
#define ECC_TYPE_24BIT 0x3
#define ECC_TYPE_28BIT 0x4
#define ECC_TYPE_40BIT 0x5
#define ECC_TYPE_64BIT 0x6

#define PAGE_SIZE_2KB 0x0
#define PAGE_SIZE_4KB 0x1
#define PAGE_SIZE_8KB 0x2
#define PAGE_SIZE_16KB 0x3

struct _patch_data_t {
	uint32_t reg;
	uint32_t value;
};

struct _spi_nand_info {
	uint16_t id;
	uint16_t page_size;

	uint16_t spare_size;
	uint16_t pages_per_block;

	uint16_t block_cnt; // up to 32k block
	uint8_t pages_per_block_shift;
	uint8_t flags;
#ifdef NANDBOOT_V2
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_valid_val;
#endif
};

struct _spi_nand_base_vector_t {
	uint32_t signature;
	uint32_t version;

	uint16_t spi_nand_vector_blks[SPI_NAND_BASE_DATA_BACKUP_COPY];

	uint16_t fip_bin_blk_cnt;
	uint16_t fip_bin_blks[SPI_NAND_BASE_DATA_BACKUP_COPY]
			     [SPI_NAND_FIP_DATA_BLOCK_COUNT];

	uint16_t erase_count; // erase count for sys base block
	uint16_t rsvd_block_count; // how many blocks reserved for spi_nand_vect and fip.bin
	uint32_t spi_nand_vector_length; // spi_nand vector struct length, must less than a page

	uint8_t spi_nand_base_block_usage
		[SPI_NAND_BASE_BLOCK_MAX_COUNT_ENTRY]; // 32 bits => 1 DW

	struct _spi_nand_info spi_nand_info;

	uint8_t factory_defect_buf[MAX_BLOCK_CNT /
				   8]; // factory defect block table, up to 512 bytes

	uint32_t bld_size;
	uintptr_t bld_loading_to_addr;

	uint32_t valid_patch_num;
	struct _patch_data_t patch_data[NUMBER_PATCH_SET];
#ifdef NANDBOOT_V2
	uint16_t crc;
#else
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_valid_val;
#endif
};
static inline uint32_t DESC_CONV(char *x)
{
	return ((((((x[0] << 8) | x[1]) << 8) | x[2]) << 8) | x[3]);
}
#endif
