// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdlib.h>
#include <common.h>
#include <config.h>
#include <command.h>
#include <fs.h>
#include <part.h>
#include <vsprintf.h>
#include <u-boot/md5.h>
#include <image-sparse.h>
#include <div64.h>
#include <linux/math64.h>
#include <linux/log2.h>
#include <u-boot/crc.h>
#include <time.h>
#include <rand.h>
#include <env.h>

#define cvi_assert(expr) do {\
		if (!(expr)) {\
			printf("%s:%d[%s]\n", __FILE__, __LINE__, __func__);\
			while \
				(1);\
		} \
	} while (0)

#include <cvsnfc.h>

//#define CVI_SD_UPDATE_DEBUG

#ifdef CVI_SD_UPDATE_DEBUG
#define DEBUG_WRITE 1
#define DEBUG_READ  1
#define DEBUG_ERASE 1
#define DEBUG_CMD_FLOW 1
#define DEBUG_DUMP_FIP 1
#else
#define DEBUG_WRITE 0
#define DEBUG_READ  0
#define DEBUG_ERASE 0
#define DEBUG_CMD_FLOW 0
#define DEBUG_DUMP_FIP 1
#endif

#define FORCE_ROM_USE_2K_PAGE
#define PLAT_BM_FIP_MAX_SIZE	0xA0000    // 640KB, Fixed, don't change unless you know it
#define PLAT_BM_FIP_MAX_CHECK_SIZE	0xA0000    // 640KB, Fixed, don't change unless you know it

//------------------------------------------------------------------------------
//  data type definitions: typedef, struct or class
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  constant definitions:
//------------------------------------------------------------------------------
#define FIP_IMAGE_HEAD	"FIPH" /* FIP Image Header 1 */
#define FIP_IMAGE_BODY	"FIPB" /* FIP Image body */

#define FIP_MAGIC_NUMBER "CVBL01\n\0"

#define SPI_NAND_VERSION (0x1823a001)

/* system vector related definitions */
#define SPI_NAND_BASE_DATA_BACKUP_COPY (2)

#define MAX_BLOCK_CNT			(20)
/* system vector relocated definitions end here */

#define SPI_NAND_FIP_RSVD_BLOCK_COUNT MAX_BLOCK_CNT // Reserved blocks for FIP.

#define BACKUP_FIP_START_POSITION	9

#define BIT(nr)			(1UL << (nr))

#define PTR_INC(base, offset) (void *)((uint8_t *)(base) + (offset))
#define GET_PG_IDX_IN_BLK(x, y) ((x) % (y))
#define GET_BLK_IDX(x, y) ((x) / (y))

#define FIP_STOP_RECOVERY	2
#define FIP_CHECKNUM_ERROR	1
#define FIP_HEADER_NO_ISSUE	0
#define FIP_PAGE_ERROR	1
#define FIP_PAGE_NO_ISSUE	0
#define FIP_NO_AVAILABLE_BLOCK 1

struct _spi_nand_info_t {
	uint32_t version;
	uint32_t id;
	uint32_t page_size;

	uint32_t spare_size;
	uint32_t block_size;
	uint32_t pages_per_block;

	uint32_t fip_block_cnt;
	uint8_t pages_per_block_shift;
	uint8_t badblock_pos;
	uint8_t dummy_data1[2];
	uint32_t flags;
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_uncorr_val;
	uint8_t dummy_data2[2];
	uint32_t erase_count; // erase count for sys base block
	uint8_t sck_l;
	uint8_t sck_h;
	uint16_t max_freq;
	uint32_t sample_param;
	uint8_t xtal_switch;
	uint8_t dummy_data3[71];
};

#define FIP_BK_TAG_SIZE	4
/* the header should start with FIH1, FIB1+sequence number */
struct block_header_t {
	uint8_t tag[FIP_BK_TAG_SIZE];
	uint32_t bk_cnt_or_seq; /* for first block, it is block count. Otherwise, it is sequence number */
	uint32_t checknum; /* the check number to make sure all fip header and body are consistent */
	uint32_t dummy;
};

struct _fip_param1_t {
	uint64_t magic1;
	uint32_t magic2;
	uint32_t param_cksum;
	struct _spi_nand_info_t nand_info;
};

struct _fip_info_t {
	uint8_t bk_position[SPI_NAND_FIP_RSVD_BLOCK_COUNT / 2];
	uint32_t checknum[SPI_NAND_FIP_RSVD_BLOCK_COUNT / 2];
	uint8_t total_count;
	uint16_t current_bk_mask;
};

struct _spi_nand_info_t *g_spi_nand_info;
struct _fip_param1_t *g_fip_param;
struct _spi_nand_info_t spinand_info;
static char spi_nand_defect_bk_list[(MAX_BLOCK_CNT / 8) + 1];
static char spi_nand_bk_usage_list[(MAX_BLOCK_CNT / 8) + 1];

static uint8_t spi_nand_g_param_init = 0xff;
uint8_t *pg_buf;
uint32_t g_virgin_start;

//int spi_nand_flush_vec(void);
//void spi_nand_dump_vec(void);

static inline u32 DESC_CONV(char *x)
{
	return ((((((x[0] << 8) | x[1]) << 8) | x[2]) << 8) | x[3]);
}

static inline u32 CHECK_MASK_BIT(void *_mask, u32 bit)
{
	u32 w = bit / 8;
	u32 off = bit % 8;

	return ((u8 *)_mask)[w] & (1 << off);
}

static inline void SET_MASK_BIT(void *_mask, u32 bit)
{
	u32 byte = bit / 8;
	u32 offset = bit % 8;
	((u8 *)_mask)[byte] |= (1 << offset);
}

static void get_spi_nand_info(void)
{
	struct cvsnfc_host *host = cvsnfc_get_host();

	spinand_info.version = SPI_NAND_VERSION;
	spinand_info.id = host->nand_chip_info->id[0]
					| host->nand_chip_info->id[1] << 8
					| host->nand_chip_info->id[2] << 16;
	spinand_info.page_size = host->nand_chip_info->pagesize;

	spinand_info.spare_size = host->nand_chip_info->oobsize;
	spinand_info.block_size = host->nand_chip_info->erasesize;

	spinand_info.pages_per_block = (host->nand_chip_info->erasesize /
				       host->nand_chip_info->pagesize);

	spinand_info.badblock_pos = host->nand_chip_info->badblock_pos;

	spinand_info.fip_block_cnt = SPI_NAND_FIP_RSVD_BLOCK_COUNT;

	spinand_info.pages_per_block_shift = ilog2(spinand_info.pages_per_block);

	spinand_info.flags = host->nand_chip_info->flags;

	if (DEBUG_WRITE) {
		printf("NAND_DEBUG: %s NAND id=0x%x, page size=%d, page per block=%d, block_size=%d, fip_block_cnt=%d\n",
		       __func__,
	       spinand_info.id,
	       spinand_info.page_size,
	       spinand_info.pages_per_block,
	       spinand_info.block_size,
	       spinand_info.fip_block_cnt);
	}

	// TODO: get ecc info form nand_chip_info
	spinand_info.ecc_en_feature_offset = host->nand_chip_info->ecc_en_feature_offset;
	spinand_info.ecc_en_mask = host->nand_chip_info->ecc_en_mask;
	spinand_info.ecc_status_offset = host->nand_chip_info->ecc_status_offset;
	spinand_info.ecc_status_mask = host->nand_chip_info->ecc_status_mask;
	spinand_info.ecc_status_shift = host->nand_chip_info->ecc_status_shift;
	spinand_info.ecc_status_uncorr_val = host->nand_chip_info->ecc_status_uncorr_val;
	spinand_info.sck_l = host->nand_chip_info->sck_l;
	spinand_info.sck_h = host->nand_chip_info->sck_h;
	spinand_info.max_freq = host->nand_chip_info->max_freq;
	spinand_info.sample_param = host->nand_chip_info->sample_param;
	spinand_info.xtal_switch = host->nand_chip_info->xtal_switch;

	if (DEBUG_WRITE) {
		printf("NAND_DEBUG: %s, ecc_en_feature_offset = 0x%x, ecc_status_offset = 0x%x\n", __func__,
		       spinand_info.ecc_en_feature_offset, spinand_info.ecc_status_offset);

		printf("NAND_DEBUG: %s, sck_l = 0x%x, sck_h = 0x%x, max_freq = %d, sample_param=0x%x\n", __func__,
		       spinand_info.sck_l, spinand_info.sck_h, spinand_info.max_freq, spinand_info.sample_param);

		printf("NAND_DEBUG: %s, xtal_switch = %d\n", __func__, spinand_info.xtal_switch);
	}
}

static void spi_nand_global_param_init(void)
{
	struct _spi_nand_info_t *info = &spinand_info;

	if (spi_nand_g_param_init == 1) /* global parameters is initialized */
		return;

	g_fip_param = (struct _fip_param1_t *)malloc(sizeof(struct _fip_param1_t));
	memset(g_fip_param, 0, sizeof(struct _fip_param1_t));

	g_spi_nand_info = &g_fip_param->nand_info;

	get_spi_nand_info();

	pg_buf = (uint8_t *)malloc(info->page_size + info->spare_size);

	spi_nand_g_param_init = 1;

}

/*
 * Erase all block and scan defect block
 */
static int spi_nand_scan_defect(void)
{
	int status;
	struct _spi_nand_info_t *info = &spinand_info;
	struct cvsnfc_host *host = cvsnfc_get_host();

	u32 ttl_pg_sz = info->page_size + info->spare_size;

	printf("Scan and erase first %d blocks\n", info->fip_block_cnt);
//	printf("%s info =%p, page_size %d, block size=%d, spare_size=%d\n", __func__, info,
//		info->page_size, info->block_size, info->spare_size);

	memset(spi_nand_defect_bk_list, 0, (MAX_BLOCK_CNT / 8) + 1);
	memset(spi_nand_bk_usage_list, 0, (MAX_BLOCK_CNT / 8) + 1);

	for (u32 blk_id = 0; blk_id < info->fip_block_cnt; blk_id++) {
		uint32_t pg = blk_id << info->pages_per_block_shift;

		host->addr_value[1] = pg;
		cvsnfc_send_cmd_erase(host);

		memset(pg_buf, 0xff, ttl_pg_sz);

		status = cvsnfc_read_page_raw(host->mtd, host->chip, pg_buf, 0, pg);

		if (status) {
			printf("read status %d, ", status);
			cvi_assert(0);
		}

		u8 *mark = (u8 *)pg_buf;
		/* Should we check spare data at 800H of this page instead of byte 0 */
		//printf("NAND_DEBUG: block[%d] spare byte[0] at 0x800 = %x\n", blk_id, *(mark + info->page_size));

		if ((*mark != 0xff) || (*(mark + info->page_size) != 0xff)) {
			printf("\n\nFound bad block %d, ", blk_id);
			//printf("bad ");
			//printf(" mark : 0x%x\n\n", *mark);
			//bbt_dump_buf("data:", pg_buf, 16);
			SET_MASK_BIT(spi_nand_defect_bk_list, blk_id);
		}
	}

	return 0;
}

static int spi_nand_blk_allocator(int fip_idx)
{
	struct _spi_nand_info_t *info = &spinand_info;
	int blk_idx;
	u8 start_blk = 0;

	if (fip_idx == 1) {
		start_blk = BACKUP_FIP_START_POSITION;

		for (blk_idx = start_blk; blk_idx < info->fip_block_cnt; blk_idx++)
			if ((CHECK_MASK_BIT(spi_nand_defect_bk_list, blk_idx) == 0) &&
			    (CHECK_MASK_BIT(spi_nand_bk_usage_list, blk_idx) == 0))
				break;
		if (blk_idx >= info->fip_block_cnt) {
			printf("no available block can be allocated for fip[1]\n");
			return -FIP_NO_AVAILABLE_BLOCK;
		}
	} else {
		for (blk_idx = start_blk; blk_idx < BACKUP_FIP_START_POSITION; blk_idx++)
			if ((CHECK_MASK_BIT(spi_nand_defect_bk_list, blk_idx) == 0) &&
			    (CHECK_MASK_BIT(spi_nand_bk_usage_list, blk_idx) == 0))
				break;
		if (blk_idx >= BACKUP_FIP_START_POSITION) {
			printf("no available block can be allocated for fip[0]\n");
			return -FIP_NO_AVAILABLE_BLOCK;
		}
	}

	SET_MASK_BIT(spi_nand_bk_usage_list, blk_idx);

	return blk_idx;
}

int check_fip_checknum(struct _fip_info_t *fip_info)
{
	int i;

	if (fip_info->total_count != 0xff) {
		for (i = 1; i < fip_info->total_count; i++) {
			if (fip_info->checknum[0] == fip_info->checknum[i])
				continue; /* checknum is consistent with fip header */
			else
				return 1; /* checknum is not consistent, it means fip is corrupted */
		}
		return 0; /* all checknums are consistence */
	}

	return 1; /* no valid total_count, it means fip is corrupted */
}

int fip_reallocate_and_recovery(struct _fip_info_t *fip, int fip_idx, int bk_idx)
{
	struct _spi_nand_info_t *info = &spinand_info;
	struct cvsnfc_host *host = cvsnfc_get_host();
	int status;
	void *temp_buf;
	uint8_t bk_shift;
	int bk_page_id;
	int target_page_id;
	int blk_id;

	bk_shift = info->pages_per_block_shift;

	temp_buf = (uint8_t *)malloc(info->page_size);

	/* erase original block first */
	host->addr_value[1] = (fip + fip_idx)->bk_position[bk_idx] << bk_shift;
	cvsnfc_send_cmd_erase(host);

retry:
	blk_id = spi_nand_blk_allocator(fip_idx);

	if (blk_id < 0)
		return -1;

	(fip + fip_idx)->bk_position[bk_idx] = blk_id;

	for (int pg_idx = 0; pg_idx < info->pages_per_block; pg_idx++) {
		if (fip_idx == 0) {
			bk_page_id = ((fip + 1)->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;
			target_page_id = (fip->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;
		} else {
			bk_page_id = (fip->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;
			target_page_id = ((fip + 1)->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;
		}

		status = cvsnfc_read_page(host->mtd, host->chip, (void *)pg_buf,
					  info->page_size, bk_page_id);

		if (status < 0) {
			printf("%s, ECC UNCORR detect on backup bk %d page %d, stop recovery\n", __func__,
			       (fip + fip_idx)->bk_position[bk_idx], pg_idx);
			goto retry;
		}

		cvsnfc_write_page(host->mtd, host->chip, (void *)pg_buf, 0,
				  target_page_id);

		status = cvsnfc_read_page(host->mtd, host->chip, (void *)temp_buf, info->page_size,
					  target_page_id);

		if (status < 0) {
			printf("%s, ECC UNCORR detect on bk %d page %d after recovery\n", __func__,
			       (fip + fip_idx)->bk_position[bk_idx], pg_idx);
		}

		if (memcmp(temp_buf, pg_buf, info->page_size)) {
			printf("%s, fip 0 read back compare error at bk %d!\n", __func__,
			       (fip + fip_idx)->bk_position[bk_idx]);
		}

	}

	free(temp_buf);
	return 0;
}

int fip_block_recover(struct _fip_info_t *fip, int bk_idx, int backup_fip, int target_fip)
{
	struct cvsnfc_host *host = cvsnfc_get_host();
	struct _spi_nand_info_t *info = &spinand_info;
	int status;
	void *temp_buf;
	int backup_page_id;
	int target_page_id;

	temp_buf = (uint8_t *)malloc(info->page_size);
	for (int pg_idx = 0; pg_idx < info->pages_per_block; pg_idx++) {

		backup_page_id = ((fip + backup_fip)->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;
		target_page_id = ((fip + target_fip)->bk_position[bk_idx] << info->pages_per_block_shift) + pg_idx;

		status = cvsnfc_read_page(host->mtd, host->chip, (void *)pg_buf,
					  info->page_size, backup_page_id);

		if (status < 0) {
			printf("%s, ECC UNCORR detect on backup bk %d page %d, stop recovery\n",
			       __func__, (fip + backup_fip)->bk_position[bk_idx], pg_idx);
			goto recover_fail;
		}

		cvsnfc_write_page(host->mtd, host->chip, (void *)pg_buf, 0,
				  target_page_id);

		status = cvsnfc_read_page(host->mtd, host->chip, (void *)temp_buf, info->page_size,
					  target_page_id);

		if (status < 0) {
			printf("%s, ECC UNCORR detect on bk %d page %d after recovery\n",
			       __func__, (fip + target_fip)->bk_position[bk_idx], pg_idx);
			if (fip_reallocate_and_recovery(fip, target_fip, bk_idx) < 0)
				goto recover_fail;
		}

		if (memcmp(temp_buf, pg_buf, info->page_size)) {
			printf("%s, fip 0 read back compare error at bk %d!\n", __func__,
			       fip->bk_position[bk_idx]);
			if (fip_reallocate_and_recovery(fip, target_fip, bk_idx) < 0)
				goto recover_fail;
		}
	}

	free(temp_buf);
	return FIP_PAGE_NO_ISSUE;

recover_fail:
	free(temp_buf);
	return FIP_STOP_RECOVERY;
}

int fip_check_checknum_and_recovery(struct _fip_info_t *fip)
{
	struct cvsnfc_host *host = cvsnfc_get_host();
	struct _spi_nand_info_t *info = &spinand_info;
	int status;
	int bk_id;

	if (check_fip_checknum(fip) && !check_fip_checknum(fip + 1)) {
		/* checknums within fip_info[0] are not consistnece, but fip_info[1] is OK. recovery fip0 from fip1 */
		if (fip->current_bk_mask != (fip + 1)->current_bk_mask) {
			for (int i = 0; i < (fip + 1)->total_count; i++) {
				if (!((fip->current_bk_mask >> i) & 0x1)) {
					printf("fip[0] bk idx %d is corrupted, recover from fip[1]\n", i);

					bk_id = spi_nand_blk_allocator(0);
					fip->bk_position[i] = bk_id;
					host->addr_value[1] = fip->bk_position[i] << info->pages_per_block_shift;
					cvsnfc_send_cmd_erase(host);
					status = fip_block_recover(fip, i, 1, 0);
					if (status)
						return status;
				}
			}

			fip->total_count = (fip + 1)->total_count;
			fip->current_bk_mask = (fip + 1)->current_bk_mask;
		} else {
			printf("Checknums of fip[0] are not consistence\n");

			for (int i = 0; i < fip->total_count; i++) {
				/* Erase original blocks first */
				host->addr_value[1] = fip->bk_position[i] << info->pages_per_block_shift;
				cvsnfc_send_cmd_erase(host);

				/* Read pages from backup fip then write to target pages */

				status = fip_block_recover(fip, i, 1, 0);
				if (status)
					return status;
			}
		}
	} else if (!check_fip_checknum(fip) && check_fip_checknum(fip + 1)) {
		/* checknums within fip_info[1] are not consistnece, but fip_info[0] is OK. recovery fip1 from fip0 */
		if (fip->current_bk_mask != (fip + 1)->current_bk_mask) {
			for (int i = 0; i < fip->total_count; i++) {
				if (!(((fip + 1)->current_bk_mask >> i) & 0x1)) {
					printf("fip[1] bk idx %d is corrupted, recover from fip[0]\n", i);

					bk_id = spi_nand_blk_allocator(1);
					(fip + 1)->bk_position[i] = bk_id;
					host->addr_value[1] = (fip + 1)->bk_position[i] << info->pages_per_block_shift;
					cvsnfc_send_cmd_erase(host);
					status = fip_block_recover(fip, i, 0, 1);
					if (status)
						return status;
				}
			}
			(fip + 1)->total_count = fip->total_count;
			(fip + 1)->current_bk_mask = fip->current_bk_mask;
		} else {
			printf("Checknums of fip[1] are not consistence\n");

			for (int i = 0; i < (fip + 1)->total_count; i++) {
				/* Erase original blocks first */
				host->addr_value[1] = (fip + 1)->bk_position[i] << info->pages_per_block_shift;
				cvsnfc_send_cmd_erase(host);

				/* Read pages from backup fip then write to target pages */
				status = fip_block_recover(fip, i, 0, 1);
				if (status)
					return status;
			}
		}
	} else if (check_fip_checknum(fip) && check_fip_checknum(fip + 1)) {
		printf("Both fip[0] and fip[1] are corrupted, please do re-download again!!\n");
		return FIP_STOP_RECOVERY;
	} else if (fip->checknum[0] != (fip + 1)->checknum[0]) {
		/* checknums of fip0 and fip1 are not the same, it means fip download is not complete*/
		/* recovery fip0 from fip1 */
		printf("Checknum of fip[0] is not the same with fip[1]\n");
		for (int i = 0; i < (fip + 1)->total_count; i++) {

			/* Erase original blocks first */
			host->addr_value[1] = fip->bk_position[i] << info->pages_per_block_shift;
			cvsnfc_send_cmd_erase(host);

			/* Read pages from backup fip then write to target pages */
			status = fip_block_recover(fip, i, 1, 0);
			if (status)
				return status;
		}

		fip->total_count = (fip + 1)->total_count;
		fip->current_bk_mask = (fip + 1)->current_bk_mask;
	} else {
		printf("All Header check PASS\n");
		return FIP_HEADER_NO_ISSUE;
	}

	return FIP_CHECKNUM_ERROR;
}

int fip_check_page_and_recovery(struct _fip_info_t *fip)
{
	struct _spi_nand_info_t *info = &spinand_info;
	struct cvsnfc_host *host = cvsnfc_get_host();
	int status;
	uint8_t	erase_and_recover = 0;
	uint8_t fip_idx = 0;
	uint8_t bk_shift;

	bk_shift = info->pages_per_block_shift;

	for (fip_idx = 0; fip_idx < SPI_NAND_BASE_DATA_BACKUP_COPY; fip_idx++) {

		for (int i = 0; i < fip->total_count; i++) {
			for (int pg_idx = 0; pg_idx < info->pages_per_block; pg_idx++) {
				int page_id;

				page_id = ((fip + fip_idx)->bk_position[i] << bk_shift) + pg_idx;
				status = cvsnfc_read_page(host->mtd, host->chip, (void *)pg_buf,
							  info->page_size, page_id);

				if (status < 0) {
					printf("ECC UNCORR detect on bk %d page %d\n",
					       (fip + fip_idx)->bk_position[i], pg_idx);
					erase_and_recover = 1;
					break; /* need to erase a whole block and then re-write again */
				}
			}

			if (erase_and_recover == 1) {
				printf("Recover bk %d due to page error\n", (fip + fip_idx)->bk_position[i]);
				host->addr_value[1] = (fip + fip_idx)->bk_position[i] << bk_shift;
				cvsnfc_send_cmd_erase(host);
				if (fip_idx == 0)
					status = fip_block_recover(fip, i, 1, 0);
				else
					status = fip_block_recover(fip, i, 0, 1);

				if (status)
					return status;

				erase_and_recover = 0;
			}
		}
	}

	printf("All Page check PASS\n");
	return 0;
}

/*
 * This function is entry point of normal bootup of u-boot
 * check each block status and recovery it if ECC ERROR occur
 */
int check_and_update_fip_bin(void)
{
	struct _spi_nand_info_t *info;
	struct cvsnfc_host *host = cvsnfc_get_host();
	u32 blk_id = 0;
	struct _fip_info_t fip_info[SPI_NAND_BASE_DATA_BACKUP_COPY];
	struct block_header_t *bk_header;
	u32 pg_sz, bk_sz;
	int status;
	u8	fip_header_count = 0;
	u32 boot_src;

	spi_nand_global_param_init();

	info = &spinand_info;

	pg_sz = info->page_size;
	bk_sz = info->block_size;

	memset(pg_buf, 0, sizeof(pg_sz));
	memset(&fip_info, 0xff, sizeof(struct _fip_info_t) * SPI_NAND_BASE_DATA_BACKUP_COPY);
	for (int i = 0; i < SPI_NAND_BASE_DATA_BACKUP_COPY; i++) {
		fip_info[i].current_bk_mask = 0x0;
	}
	memset(spi_nand_defect_bk_list, 0, (MAX_BLOCK_CNT / 8) + 1);
	memset(spi_nand_bk_usage_list, 0, (MAX_BLOCK_CNT / 8) + 1);

	for (blk_id = 0; blk_id < SPI_NAND_FIP_RSVD_BLOCK_COUNT; blk_id++) {

		status = cvsnfc_read_page(host->mtd, host->chip, (void *)pg_buf, pg_sz,
					  (blk_id << info->pages_per_block_shift) + 0);
		/* Read first page of each block */

		if (status < 0) {
			printf("%s, find ECC UNCORR on bk %d\n", __func__, blk_id);
			SET_MASK_BIT(spi_nand_defect_bk_list, blk_id);
			continue;
		}

		bk_header = (struct block_header_t *)pg_buf;

		if (!memcmp(bk_header->tag, FIP_IMAGE_HEAD, FIP_BK_TAG_SIZE)) {
			fip_header_count++;
			if (fip_header_count == 1 && blk_id >= BACKUP_FIP_START_POSITION)
				printf("WARNING!! First fip header position exceed backup start position\n");

			if (fip_header_count == 1)
				spi_nand_adjust_max_freq(host, (u32 *)bk_header->tag, &bk_header->checknum);

			if (blk_id < BACKUP_FIP_START_POSITION) {
				if (fip_info[0].bk_position[0] == 0xff) {
					fip_info[0].bk_position[0] = blk_id;
					fip_info[0].checknum[0] = bk_header->checknum;
					fip_info[0].total_count = bk_header->bk_cnt_or_seq;
					fip_info[0].current_bk_mask |= 0x1 << 0;
					SET_MASK_BIT(spi_nand_bk_usage_list, blk_id);
					continue;
				} else
					printf("fip info[0] header is not initialized before\n");
			} else {
				if (fip_info[1].bk_position[0] == 0xff) {
					fip_info[1].bk_position[0] = blk_id;
					fip_info[1].checknum[0] = bk_header->checknum;
					fip_info[1].total_count = bk_header->bk_cnt_or_seq;
					fip_info[1].current_bk_mask |= 0x1 << 0;
					SET_MASK_BIT(spi_nand_bk_usage_list, blk_id);
					continue;
				} else
					printf("fip info[1] header is not initialized before\n");
			}
			if (fip_header_count > SPI_NAND_BASE_DATA_BACKUP_COPY)
				printf("%s, WARNING!! find unexpected fip_header at bk %d\n", __func__, blk_id);
		} else if (!memcmp(bk_header->tag, FIP_IMAGE_BODY, FIP_BK_TAG_SIZE)) {
			if (blk_id < BACKUP_FIP_START_POSITION) {
				if (fip_info[0].bk_position[bk_header->bk_cnt_or_seq] == 0xff) {
					fip_info[0].bk_position[bk_header->bk_cnt_or_seq] = blk_id;
					fip_info[0].checknum[bk_header->bk_cnt_or_seq] = bk_header->checknum;
					fip_info[0].current_bk_mask |= 0x1 << bk_header->bk_cnt_or_seq;
					SET_MASK_BIT(spi_nand_bk_usage_list, blk_id);
					continue;
				} else
					printf("fip info[0] body %d is not initialized before\n",
					       bk_header->bk_cnt_or_seq);
			} else {
				if (fip_info[1].bk_position[bk_header->bk_cnt_or_seq] == 0xff) {
					fip_info[1].bk_position[bk_header->bk_cnt_or_seq] = blk_id;
					fip_info[1].checknum[bk_header->bk_cnt_or_seq] = bk_header->checknum;
					fip_info[1].current_bk_mask |= 0x1 << bk_header->bk_cnt_or_seq;
					SET_MASK_BIT(spi_nand_bk_usage_list, blk_id);
					continue;
				} else
					printf("fip info[1] body %d is not initialized before\n",
					       bk_header->bk_cnt_or_seq);
			}

		}
	}

	if (DEBUG_DUMP_FIP) {
		for (int i = 0; i < SPI_NAND_BASE_DATA_BACKUP_COPY; i++) {
			printf("fip_info[%d] total count=%d\n", i, fip_info[i].total_count);

			for (int j = 0; j < (SPI_NAND_FIP_RSVD_BLOCK_COUNT / 2); j++)
				printf("fip_info[%d].bk_position[%d]= 0x%x, fip_info[%d].checknum[%d] = 0x%x\n",
				       i, j, fip_info[i].bk_position[j], i, j, fip_info[i].checknum[j]);
			printf("fip_info[%d].current mask=0x%x\n", i, fip_info[i].current_bk_mask);
		}

		printf("defect list = %02x%02x%02x\n", spi_nand_defect_bk_list[2],
		       spi_nand_defect_bk_list[1],  spi_nand_defect_bk_list[0]);
		printf("usage list = %02x%02x%02x\n", spi_nand_bk_usage_list[2],
		       spi_nand_bk_usage_list[1], spi_nand_bk_usage_list[0]);
	}

	boot_src = readl((unsigned int *)BOOT_SOURCE_FLAG_ADDR);

	if (boot_src == MAGIC_NUM_SD_DL || boot_src == MAGIC_NUM_USB_DL)
		return 0; /* No need to check and update spinand */

	if (fip_header_count <= SPI_NAND_BASE_DATA_BACKUP_COPY) {
		if (!fip_check_checknum_and_recovery(fip_info)) {
			/* check page ecc and do revoery if necessary */
			fip_check_page_and_recovery(fip_info);
		}
//	} else if (fip_header_count == 1)	{
//		printf("Only detect 1 fip header\n");
	} else if (fip_header_count > SPI_NAND_BASE_DATA_BACKUP_COPY) {
		printf("WARNING!! find unexpected fip_header\n");
	} else {
		printf("WARNING!! cannot find fip_header\n");
	}

	return 0;
}

static uint32_t spi_nand_crc16_ccitt_with_tag(unsigned char *buf, int len)
{
	uint32_t crc = 0;

	crc = crc16_ccitt(0, buf, len);
	crc |= 0xCAFE0000;

	return crc;
}

static void spi_nand_check_size_error(u32 target_len, u32 curr_offset, u32 pg_size)
{
	if ((target_len - curr_offset) >= pg_size)
		printf("## WARNING 1 ## data size %d to be wrote is wrong!!\n", (target_len - curr_offset));
}

static int spi_nand_flush_fip_bin(void *buf)
{
	int status = 0;
	void *buffer = (void *)buf;
	void *src_buf_addr = 0;
	struct cvsnfc_host *host = cvsnfc_get_host();
	struct _spi_nand_info_t *info = &spinand_info;
	u32 pg_sz, pg_per_blk, bk_sz;
	u32 total_len, ttl_block_cnt_to_write, ttl_pg_cnt_to_write;
	struct block_header_t bk_header;
	u32 bk_overhead; /* used to  calculate total block header size in page 0 of each block */
	u32 blk_idx = 0;
	u32 blk_id = 0;
	u32 src_len;
	uint8_t *temp_buf;
	unsigned int checknum;

	srand(get_timer(0));
	checknum = rand();

	printf("Generated checknum=0x%x\n", checknum);

	pg_sz = info->page_size;
	bk_sz = info->block_size;
	src_len = env_get_ulong("filesize", 16, 0);

	printf("fip size=%d bytes\n", src_len);

	ttl_block_cnt_to_write = src_len / bk_sz;
	if ((src_len % bk_sz) != 0)
		ttl_block_cnt_to_write += 1;

	bk_overhead = sizeof(struct block_header_t);
	memset(&bk_header, 0, sizeof(struct block_header_t));

	total_len = (src_len + (ttl_block_cnt_to_write * bk_overhead));

	ttl_pg_cnt_to_write = total_len / pg_sz;
	if (total_len % pg_sz != 0)
		ttl_pg_cnt_to_write += 1; /* add 1 page to write remaining data */

	ttl_block_cnt_to_write = total_len / bk_sz; /* re-calculate new block count */
	if (total_len % bk_sz != 0)
		ttl_block_cnt_to_write += 1;

	if (DEBUG_WRITE)
		printf("Write totol_len=%d, bk_overhead=%d, ttl_page_cnt_to_write=%d, ttl_block_cnt_to_write=%d\n",
		       total_len, bk_overhead, ttl_pg_cnt_to_write, ttl_block_cnt_to_write);

	pg_per_blk = info->pages_per_block;

	temp_buf = (uint8_t *)malloc(info->page_size);

	for (u32 i = 0; i < SPI_NAND_BASE_DATA_BACKUP_COPY; i++) {
		u32 offset_in_buf = 0;
		uint8_t wrote_bk_cnt = 0;

		printf("write %d copy of fip\n", i + 1);

		for (u32 pg_idx_in_buf = 0; pg_idx_in_buf < ttl_pg_cnt_to_write; pg_idx_in_buf++) {
			u32 pg_idx_in_blk;

			uint8_t block_damage = 0;

			pg_idx_in_blk = GET_PG_IDX_IN_BLK(pg_idx_in_buf, pg_per_blk);
			blk_idx = GET_BLK_IDX(pg_idx_in_buf, pg_per_blk);

			src_buf_addr = PTR_INC(buffer, offset_in_buf);

			if (pg_idx_in_blk == 0) {
				if (wrote_bk_cnt == 0) { /* Fill FIP image first block header */
					struct _fip_param1_t *temp_fip_param;
					uint32_t crc = 0;
					int param_crc_size;

					memcpy(bk_header.tag, FIP_IMAGE_HEAD, 4);
					bk_header.bk_cnt_or_seq = (uint32_t)ttl_block_cnt_to_write;
					bk_header.checknum = checknum;
					memcpy(temp_buf, &bk_header, bk_overhead);
					temp_fip_param = src_buf_addr;

					memcpy(&temp_fip_param->nand_info, info, sizeof(struct _spi_nand_info_t));
					param_crc_size = 0x800 - offsetof(struct _fip_param1_t, nand_info);

					crc = spi_nand_crc16_ccitt_with_tag((unsigned char *)&temp_fip_param->nand_info
						  , param_crc_size);
					//printf("%s, get crc=0x%08x, param_crc_size=%d\n", __func__,
					//       crc, param_crc_size);
					temp_fip_param->param_cksum = crc;
					memcpy((temp_buf + bk_overhead), src_buf_addr, (pg_sz - bk_overhead));

				} else { /* Fill remaining FIP image body */
					memcpy(bk_header.tag, FIP_IMAGE_BODY, 4);
					bk_header.bk_cnt_or_seq = (uint32_t)wrote_bk_cnt;
					bk_header.checknum = checknum;
					memcpy(temp_buf, &bk_header, bk_overhead);
					if (pg_idx_in_buf == (ttl_pg_cnt_to_write - 1)) { /* last page */
						spi_nand_check_size_error(src_len, offset_in_buf, pg_sz);
						memcpy((temp_buf + bk_overhead), src_buf_addr,
						       (src_len - offset_in_buf));
					} else
						memcpy((temp_buf + bk_overhead), src_buf_addr, (pg_sz - bk_overhead));
				}
				wrote_bk_cnt++;
				offset_in_buf = offset_in_buf + (pg_sz - bk_overhead); /* Insert fip header in page 0 */
			} else {
				if (pg_idx_in_buf == (ttl_pg_cnt_to_write - 1)) { /* last page */
					spi_nand_check_size_error(src_len, offset_in_buf, pg_sz);
					memcpy(temp_buf, src_buf_addr, (src_len - offset_in_buf));
				} else
					memcpy(temp_buf, src_buf_addr, pg_sz);

				offset_in_buf = offset_in_buf + pg_sz;
			}

			if (DEBUG_WRITE)
				printf("flush fip.bin with next offset = 0x%x\n",
				       offset_in_buf);
retry:
			if (pg_idx_in_blk == 0 || block_damage == 1) {
				/* pg_idx_in_blk == 0 means need a new block */
				/* damage == 1 means need to find next block*/
				blk_id = spi_nand_blk_allocator(i);

				if (DEBUG_WRITE)
					printf("NAND_DEBUG: %s, allocate blk_id=%d for page %d (%x)\n",
					       __func__, blk_id, pg_idx_in_buf, pg_idx_in_buf);

				if (blk_id == -1) {
					cvi_assert(0);
					return -1;
				}

				block_damage = 0;
			}

			/* should we supplyment remain data of last page if remaining data is less then 1 page ?*/
			cvsnfc_write_page(host->mtd, host->chip, (void *)temp_buf, 0,
					  (blk_id << info->pages_per_block_shift) +  pg_idx_in_blk);
			/* read page again to check data consistent */
			memset(pg_buf, 0, sizeof(pg_sz));
			status = cvsnfc_read_page(host->mtd, host->chip, (void *)pg_buf, pg_sz,
						  (blk_id << info->pages_per_block_shift) + pg_idx_in_blk);

			if (status < 0) {
				printf("##WARNING## %s update failed at block %d page %d, please check...\n",
				       __func__, blk_id, pg_idx_in_blk);
				//cvi_assert(0);
				block_damage = 1;
				host->addr_value[1] = blk_id << info->pages_per_block_shift;
				cvsnfc_send_cmd_erase(host);
				SET_MASK_BIT(spi_nand_defect_bk_list, blk_id);
				goto retry;

			} else if (status > 0) {
				printf("NAND_DEBUG: Read ecc corr page idx %d, blk_id %d\n", pg_idx_in_blk, blk_id);
			}

			if (memcmp(temp_buf, pg_buf, pg_sz)) {
				printf("fip read back compare error!\n");
				bbt_dump_buf((void *)"temp_buf", temp_buf, pg_sz >> 4);
				bbt_dump_buf((void *)"pg_buf", pg_buf, pg_sz >> 4);
				cvi_assert(0);
			}
		}
	}

	free(temp_buf);

	return 0;
}

int spi_nand_fip_download(void *buf)
{
	int status;

	/* always erase fip partition first and then download re-assemble fip */

	status = spi_nand_scan_defect();
	if (status) {
		printf("scan factory error\n");
		return -1;
	}

	status = spi_nand_flush_fip_bin(buf);
	if (status)
		printf("flush fip error\n");

	return 0;
}

/*
 * This function is entry point of u-boot download process
 */
int do_cvi_update_spinand(uint32_t component, void *addr)
{
	printf("%s with version 0x%x\n", __func__, SPI_NAND_VERSION);

	if (DEBUG_WRITE)
		bbt_dump_buf("source fip", addr, 0x40);

	spi_nand_fip_download(addr);

	return 0;
}

