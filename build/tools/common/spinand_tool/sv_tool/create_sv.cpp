#include <bits/stdint-uintn.h>
#include <iostream>
#include <sys/types.h>
#include <cstring>
#include <fstream>
#include "create_sv.h"
#include "crc.h"
#include <unistd.h> /* getopt */
using std::cout;
using std::endl;
using std::ios;
using std::string;
struct _spi_nand_info spinand_info;
struct _spi_nand_base_vector_t g_spi_nand_sys_vec;
void set_spi_nand_info(struct _spi_nand_info *nand_info)
{
	nand_info->id = 0xefaa;
	nand_info->page_size = _2K;

	nand_info->spare_size = 64;
	nand_info->pages_per_block = _128K / _2K;

	nand_info->block_cnt = 32; // FIXME !!
	nand_info->pages_per_block_shift = 6; // TODO: Caculate pages_per_block
	nand_info->flags = 0;
#ifdef NANDBOOT_V2
	// TODO: get ecc info form nand_chip_info
	nand_info->ecc_en_feature_offset = 0xb0;
	nand_info->ecc_en_mask = 1 << 4;
	nand_info->ecc_status_offset = 0xc0;
	nand_info->ecc_status_mask = 0x30;
	nand_info->ecc_status_shift = 4;
	nand_info->ecc_status_valid_val = 0xff;
#else
	g_spi_nand_sys_vec.ecc_en_feature_offset = 0xb0;
	g_spi_nand_sys_vec.ecc_en_mask = 1 << 4;
	g_spi_nand_sys_vec.ecc_status_offset = 0xc0;
	g_spi_nand_sys_vec.ecc_status_mask = 0x30;
	g_spi_nand_sys_vec.ecc_status_shift = 4;
	g_spi_nand_sys_vec.ecc_status_valid_val = 0xff;
#endif
}
int init_spi_nand_vect(int fip_blks_cnt = 1)
{
	struct _spi_nand_base_vector_t *sv = &g_spi_nand_sys_vec;
	struct _spi_nand_info *info = &spinand_info;
	//	struct cvsnfc_host *host = cvsnfc_get_host();

	int i = 0, j = 0;
	int blk_idx = 0;

	memset(sv, 0, sizeof(struct _spi_nand_base_vector_t));

	sv->signature = (uint32_t)SPI_NAND_VECTOR_SIGNATURE;
	sv->version = SPI_NAND_VECTOR_VERSION;
	sv->erase_count = 1;
	sv->rsvd_block_count = SPI_NAND_BASE_BLOCK_COUNT;
	sv->spi_nand_vector_length = sizeof(struct _spi_nand_base_vector_t);
	sv->fip_bin_blk_cnt = fip_blks_cnt;
	printf("sv length %d\n", sv->spi_nand_vector_length);
	printf("fip_bin_blk_cnt  %d\n", sv->fip_bin_blk_cnt);
	//	sv->bld_size = BLD_SIZE;
	//	sv->bld_loading_to_addr = (uintptr_t) BLD_BASE;
	set_spi_nand_info(&sv->spi_nand_info);
	/* memcpy(&sv->spi_nand_info, info, sizeof(struct _spi_nand_info)); */
	// Useless parameters
	//	memcpy(sv->spi_nand_base_block_usage, spi_nand_defect_buf, SPI_NAND_BASE_BLOCK_MAX_COUNT_ENTRY);
	//	memcpy(sv->factory_defect_buf, spi_nand_defect_buf, sv->spi_nand_info.block_cnt / 8);

	//	fill_spi_nand_patch();

	printf("[SPI_NAND][BASE]alloc spi_nand vector blocks\n");
	// Alloc spi_nand vector blk
	for (i = 0; i < SPI_NAND_BASE_DATA_BACKUP_COPY; i++) {
		blk_idx = i;
		sv->spi_nand_vector_blks[i] = blk_idx;
		printf("[SPI_NAND][BASE]spi_nand vector block%d: %d\n", i,
		       blk_idx);
	}

	// Alloc fip.bin blocks
	// fip = 640KB, blk sz = 128KB
	for (i = 0; i < SPI_NAND_FIP_DATA_BACKUP_COUNT; i++) {
		for (j = 0; j < sv->fip_bin_blk_cnt; j++) {
			blk_idx = (i + 1) * 4 + j;
			sv->fip_bin_blks[i][j] = blk_idx;
			printf("[SPI_NAND][BASE] alloc fip block [%d][%d]: %d\n",
			       i, j, blk_idx);
		}
	}
#ifdef NANDBOOT_V2
	sv->crc = crc16_ccitt(0, (unsigned char *)sv,
			      (int)(uintptr_t)&sv->crc - (uintptr_t)sv);
	printf("[SPI_NAND][BASE] sv->crc:%d\n", sv->crc);
#endif
	return 0;
}
void dump_nand_info(struct _spi_nand_info *nand_info)
{
	cout << "ID:" << nand_info->id << endl;
	cout << "Page size:" << nand_info->page_size << endl;
	cout << "Spare size:" << nand_info->spare_size << endl;
	cout << "pages_per_block:" << nand_info->pages_per_block << endl;

	cout << "block count:" << nand_info->block_cnt << endl;
	cout << "pages_per_block_shift" << nand_info->pages_per_block_shift
	     << endl;
	cout << "flags" << nand_info->flags << endl;
#ifdef NANDBOOT_V2
	// TODO: get ecc info form nand_chip_info
	printf("ecc_en_feature_offset:0x%x\n",
	       nand_info->ecc_en_feature_offset);
	printf("ecc_en_mask:0x%x\n", nand_info->ecc_en_mask);
	printf("ecc_status_offset:0x%x\n", nand_info->ecc_status_offset);
	printf("ecc_status_mask:0x%x\n", nand_info->ecc_status_mask);
	printf("ecc_status_shift:%d\n", nand_info->ecc_status_shift);
	printf("ecc_status_valid_val:0x%x\n", nand_info->ecc_status_valid_val);
#endif
}
int main(int argc, char *argv[])
{
	string output = "sv.bin";
	int cmd_opt = 0;
	int fip_blks_cnt = 1;
	while (1) {
		cmd_opt = getopt(argc, argv, "o:c:");

		/* End condition always first */
		if (cmd_opt == -1) {
			break;
		}
		/* Print option when it is valid */
		if (cmd_opt != '?') {
			fprintf(stderr, "option:-%c\n", cmd_opt);
		}

		/* Lets parse */
		switch (cmd_opt) {
		/* No args */
		case 'c':
			fip_blks_cnt = atoi(optarg);
			break;
		case 'o':
			output = optarg;
			break;

		/* Error handle: Mainly missing arg or illegal option */
		case '?':
			fprintf(stderr, "Illegal option:-%c\n",
				isprint(optopt) ? optopt : '#');
			break;
		default:
			fprintf(stderr, "Not supported option\n");
			break;
		}
	}

	std::ofstream ofs(output, ios::out | ios::binary);
	init_spi_nand_vect(fip_blks_cnt);
	dump_nand_info(&g_spi_nand_sys_vec.spi_nand_info);
	cout << "Output: " << output << endl;
#ifdef NANDBOOT_V2
	printf("sv crc:%x\n", g_spi_nand_sys_vec.crc);
#endif
	char *output_buf = new char[_2K];
	memset(output_buf, 0xff, _2K);
	memcpy(output_buf, &g_spi_nand_sys_vec,
	       sizeof(_spi_nand_base_vector_t));
	for (int i = 0; i < g_spi_nand_sys_vec.spi_nand_info.pages_per_block;
	     i++) {
		ofs.write(output_buf, _2K);
	}
	ofs.close();
	return 0;
}
