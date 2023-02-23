#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct _spi_nand_info_t {
	unsigned int version;
	unsigned int id;
	unsigned int page_size;

	unsigned int spare_size;
	unsigned int block_size;
	unsigned int pages_per_block;

	unsigned int fip_block_cnt;
	unsigned char pages_per_block_shift;
	unsigned char badblock_pos;
	unsigned char dummy_data1[2];
	unsigned int flags;
	unsigned char ecc_en_feature_offset;
	unsigned char ecc_en_mask;
	unsigned char ecc_status_offset;
	unsigned char ecc_status_mask;
	unsigned char ecc_status_shift;
	unsigned char ecc_status_uncorr_val;
	unsigned char dummy_data2[2];
	unsigned int erase_count; // erase count for sys base block
	unsigned char sck_l;
	unsigned char sck_h;
	unsigned short max_freq;
	unsigned int  sample_param;
	unsigned char xtal_switch;
	unsigned char dummy_data3[71];
};

struct block_header_t {
	unsigned char tag[4];
	unsigned int bc_or_seq;
	unsigned int checknum;
	unsigned int dummy_2;
};

struct _fip_param1_t {
	unsigned long magic1; /* fip magic number*/
	unsigned int magic2;
	unsigned int param_cksum;
	struct _spi_nand_info_t nand_info;
};

#define PTR_INC(base, offset) (void *)((unsigned char *)(base) + (offset))
#define FIP_IMAGE_HEAD	"FIPH" /* FIP Image Header 1 */
#define FIP_IMAGE_BODY	"FIPB" /* FIP Image body */


#define FIP_MAGIC_NUMBER "CVBL01\n\0"

#define SPI_NAND_VERSION (0x1823a001)

#define BIT(nr)			(1UL << (nr))

#define FLAGS_SET_PLANE_BIT		(BIT(0))
#define FLAGS_SET_QE_BIT		(BIT(1))
#define FLAGS_ENABLE_X2_BIT		(BIT(2))
#define FLAGS_ENABLE_X4_BIT		(BIT(3))
#define FLAGS_OW_SETTING_BIT		(BIT(4))

#define BBP_LAST_PAGE			0x01
#define BBP_FIRST_PAGE			0x02
#define BBP_FIRST_2_PAGE		0x03

/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static const unsigned short crc16_tab[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

unsigned short crc16_ccitt(unsigned short cksum, const unsigned char *buf, int len)
{
	for (int i = 0;  i < len;  i++)
		cksum = crc16_tab[((cksum>>8) ^ *buf++) & 0xff] ^ (cksum << 8);

	return cksum;
}

static unsigned int spi_nand_crc16_ccitt_with_tag(unsigned char *buf, int len)
{
	unsigned int crc = 0;

	crc = crc16_ccitt(0, buf, len);
	crc |= 0xCAFE0000;

	return crc;
}

#define _2K			(0x800UL)
#define _4K			(0x1000UL)
#define _128K		(0x20000UL)
#define _256K		(0x40000UL)

void set_spi_nand_info(struct _spi_nand_info_t *nand)
{
	nand->version = SPI_NAND_VERSION;
	nand->id = 0xffff;
	nand->page_size = 2048;
	nand->spare_size = 64;
	nand->block_size = 0x20000;
	nand->pages_per_block = 64;
	nand->badblock_pos = BBP_FIRST_PAGE;
	nand->fip_block_cnt = 20;
	nand->pages_per_block_shift = 6;
	nand->flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT;
	nand->ecc_en_feature_offset = 0xB0;
	nand->ecc_en_mask = 1 << 4;
	nand->ecc_status_offset = 0xC0;
	nand->ecc_status_mask = 0x30;
	nand->ecc_status_shift = 4;
	nand->ecc_status_uncorr_val = 0x2;
	nand->sck_l = 1;
	nand->sck_h = 0;
	nand->max_freq = 6;
	nand->sample_param = 0x40001000;
	nand->xtal_switch = 1;
}

int main(int argc, char const *argv[])
{
	FILE *inf, *outf;
	struct stat statbuf;
	int size;
	struct _spi_nand_info_t nand;
	unsigned char *buff;
	unsigned char *out_buff;
	void *src_buf_addr;
	void *dst_buf_addr;
	struct block_header_t bk_header;
	unsigned int pg_sz, pg_per_blk, bk_sz, id;
	unsigned int total_len, ttl_block_cnt_to_write, ttl_pg_cnt_to_write, out_len;
	unsigned int bk_overhead; /* used to  calculate total block header size in page 0 of each block */
	unsigned int blk_idx = 0;
	unsigned int blk_id = 0;
	int ret = 0;

	if (argc < 5) {
		printf("Usage: ./fip_maker pagesize id input_path out_path\n");
		return -1;
	}
	memset(&nand, 0, sizeof(struct _spi_nand_info_t));
	/* default parameter */
	set_spi_nand_info(&nand);
	pg_sz = strtoul(argv[1], NULL, 0);
	if (pg_sz != _2K && pg_sz != _4K) {
		printf("do not support page size:%u\n", pg_sz);
		return -1;
	}
	bk_sz = (pg_sz == _2K) ? _128K : _256K;
	id = strtoul(argv[2], NULL, 0);
	nand.page_size = pg_sz;
	nand.block_size = bk_sz;
	nand.id = id;

	if (access(argv[3], F_OK | R_OK)) {
		printf("fip.bin is not exist at %s\n", argv[2]);
		return -1;
	}

	stat(argv[3], &statbuf);
	size = statbuf.st_size;
	printf("fip.bin size is %d\n", size);

	buff = (unsigned char *)malloc(size);
	if (!buff) {
		printf("buff is invalid\n");
		return -1;
	}

	inf = fopen(argv[3], "rb");
	if (!inf) {
		printf("open fip.bin failed\n");
		ret = -1;
		goto free_src;
	}
	fread(buff, sizeof(unsigned char), size, inf);

	outf = fopen(argv[4], "wb");

	ttl_block_cnt_to_write = size / bk_sz;
	if (size % bk_sz != 0)
		ttl_block_cnt_to_write += 1;

	bk_overhead = sizeof(struct block_header_t);
	memset(&bk_header, 0, sizeof(struct block_header_t));

	total_len = (size + (ttl_block_cnt_to_write * bk_overhead));

	out_len = (total_len / bk_sz) * bk_sz;
	if ((total_len % bk_sz) != 0)
		out_len += bk_sz;

	out_buff = (unsigned char *)malloc(out_len);
	memset(out_buff, 0xff, out_len);

	dst_buf_addr = PTR_INC(out_buff, 0);

	ttl_pg_cnt_to_write = total_len / pg_sz;
	if (total_len % pg_sz != 0)
		ttl_pg_cnt_to_write += 1; /* add 1 page to write remaining data */

	ttl_block_cnt_to_write = total_len / bk_sz; /* re-calculate new block count */
	if (total_len % bk_sz != 0)
		ttl_block_cnt_to_write += 1;

	//printf("totol_len=%d, bk_overhead=%d, ttl_page_cnt_to_write=%d, ttl_block_cnt_to_write=%d\n",
	//	total_len, bk_overhead, ttl_pg_cnt_to_write, ttl_block_cnt_to_write);

	pg_per_blk = nand.pages_per_block;

	unsigned int offset_in_buf = 0;
	unsigned char wrote_bk_cnt = 0;
	unsigned char *temp_buf;

	srand(time(NULL));
	unsigned int checknum = rand();

	temp_buf = (unsigned char *)malloc(pg_sz);
	printf("original fip size=%d, total_len with header=%d, total write size=%d\n", size, total_len, out_len);
	printf("total page to write=%d, total block to write=%d\n", ttl_pg_cnt_to_write, ttl_block_cnt_to_write);
	printf("generated check number is 0x%x\n", checknum);
	printf("Re-Assemble fip.bin to %s\n", argv[4]);

	for (unsigned int pg_idx_in_buf = 0; pg_idx_in_buf < ttl_pg_cnt_to_write; pg_idx_in_buf++) {
		unsigned int pg_idx_in_blk;

		pg_idx_in_blk = pg_idx_in_buf % pg_per_blk;
		blk_idx = pg_idx_in_buf / pg_per_blk;

		src_buf_addr = PTR_INC(buff, offset_in_buf);
		dst_buf_addr = PTR_INC(out_buff, (pg_idx_in_buf * pg_sz));

		if (pg_idx_in_blk == 0) {
			if (wrote_bk_cnt == 0) { /* Fill FIP image first block header */
				struct _fip_param1_t *tmp_fip_param;
				unsigned int crc = 0;
				int param_crc_size;

				memcpy(bk_header.tag, FIP_IMAGE_HEAD, 4);
				bk_header.bc_or_seq = (unsigned int)ttl_block_cnt_to_write;
				bk_header.checknum = checknum;
				memcpy(temp_buf, &bk_header, bk_overhead);
				tmp_fip_param = src_buf_addr;
				memcpy(&(tmp_fip_param->nand_info), &nand, sizeof(struct _spi_nand_info_t));
				param_crc_size = 0x800 - offsetof(struct _fip_param1_t, nand_info);
				crc = spi_nand_crc16_ccitt_with_tag((unsigned char *)&tmp_fip_param->nand_info
					  , param_crc_size);
				tmp_fip_param->param_cksum = crc;
				printf("get CRC = 0x%x, spare_size=0x%x\n", crc, nand.spare_size);
				memcpy((temp_buf + bk_overhead), src_buf_addr, (pg_sz - bk_overhead));
			} else { /* Fill remaining FIP image body */
				memcpy(bk_header.tag, FIP_IMAGE_BODY, 4);
				bk_header.bc_or_seq = (unsigned int)wrote_bk_cnt;
				bk_header.checknum = checknum;
				memcpy(temp_buf, &bk_header, bk_overhead);
				if (pg_idx_in_buf == (ttl_pg_cnt_to_write - 1)) { /* last page */
					if ((size - offset_in_buf) >= pg_sz)
						printf("## WARNING 1 ## data size %d to be wrote is wrong!!\n",
						       (size - offset_in_buf));
					memcpy((temp_buf + bk_overhead), src_buf_addr, (size - offset_in_buf));
				} else
					memcpy((temp_buf + bk_overhead), src_buf_addr, (pg_sz - bk_overhead));
			}
			wrote_bk_cnt++;
			offset_in_buf = offset_in_buf + (pg_sz - bk_overhead); /* Insert fip header in page 0 */
		} else {
			if (pg_idx_in_buf == (ttl_pg_cnt_to_write - 1)) { /* last page */
				if ((size - offset_in_buf) > pg_sz)
					printf("## WARNING 2 ## data size %d to be wrote is wrong!!\n",
						   (size - offset_in_buf));
				memcpy(temp_buf, src_buf_addr, (size - offset_in_buf));

			} else
				memcpy(temp_buf, src_buf_addr, pg_sz);

			offset_in_buf = offset_in_buf + pg_sz;
		}

		if (pg_idx_in_blk == 0) {
			/* pg_idx_in_blk == 0 means need a new block */
			/* damage == 1 means need to find next block*/
			blk_id++;
			printf("allocate blk_id=%d\n", (blk_id - 1));
		}

		memcpy(dst_buf_addr, temp_buf, pg_sz);

	}

	fwrite(out_buff, sizeof(unsigned char), out_len, outf);
	printf("fip.bin --> %s  is completed!!\n", argv[4]);

	fclose(inf);
	fclose(outf);

	free(temp_buf);
free_dst:
	free(out_buff);
free_src:
	free(buff);

	return ret;
}
