#ifndef __CVSNFC_COMMON_H__
#define __CVSNFC_COMMON_H__

#include <linux/mtd/rawnand.h>
#include <linux/delay.h>

/*****************************************************************************/
#define _512B					(512)
#define _1K					(1024)
#define _2K					(2048)
#define _4K					(4096)
#define _8K					(8192)
#define _16K					(16384)
#define _32K					(32768)
#define _64K					(0x10000UL)
#define _128K					(0x20000UL)
#define _256K					(0x40000UL)
#define _512K					(0x80000UL)
#define _1M					(0x100000UL)
#define _2M					(0x200000UL)
#define _4M					(0x400000UL)
#define _8M					(0x800000UL)
#define _16M					(0x1000000UL)
#define _32M					(0x2000000UL)
#define _64M					(0x4000000UL)
#define _128M					(0x8000000UL)
#define _256M					(0x10000000UL)
#define _512M					(0x20000000UL)
#define _1G					(0x40000000ULL)
#define _2G					(0x80000000ULL)
#define _4G					(0x100000000ULL)
#define _8G					(0x200000000ULL)
#define _16G					(0x400000000ULL)
#define _64G					(0x1000000000ULL)

/*****************************************************************************/
#define NAND_PAGE_512B                   0
#define NAND_PAGE_1K                     1
#define NAND_PAGE_2K                     2
#define NAND_PAGE_4K                     3
#define NAND_PAGE_8K                     4
#define NAND_PAGE_16K                    5
#define NAND_PAGE_32K                    6

/*****************************************************************************/
#define NAND_ECC_NONE                    0
#define NAND_ECC_0BIT                    0
#define NAND_ECC_1BIT                    1
#define NAND_ECC_1BIT_512                1
#define NAND_ECC_4BIT                    2
#define NAND_ECC_4BIT_512                2
#define NAND_ECC_4BYTE                   2
#define NAND_ECC_8BIT                    2
#define NAND_ECC_8BIT_512                3
#define NAND_ECC_8BYTE                   3
#define NAND_ECC_13BIT                   4
#define NAND_ECC_16BIT                   5
#define NAND_ECC_18BIT                   6
#define NAND_ECC_24BIT                   7
#define NAND_ECC_27BIT                   8
#define NAND_ECC_28BIT                   9
#define NAND_ECC_32BIT                   10
#define NAND_ECC_40BIT                   11
#define NAND_ECC_41BIT                   12
#define NAND_ECC_42BIT                   13
#define NAND_ECC_48BIT                   14
#define NAND_ECC_60BIT                   15
#define NAND_ECC_64BIT                   16
#define NAND_ECC_72BIT                   17
#define NAND_ECC_80BIT                   18

#define ERSTR_HARDWARE                   "Hardware configuration error."
#define ERSTR_DRIVER                     "Driver does not support."

#define DISABLE                          0
#define ENABLE                           1

#if defined(CONFIG_NAND_FLASH_CVSNFC)
/*****************************************************************************/

struct match_reg_type {
	int reg;
	int type;
};

struct match_type_str {
	int type;
	const char *str;
};

enum ecc_type {
	et_ecc_none    = 0x00,
	et_ecc_1bit    = 0x01,
	et_ecc_4bit    = 0x02,
	et_ecc_8bit    = 0x03,
	et_ecc_24bit1k = 0x04,
	et_ecc_40bit1k = 0x05,
	et_ecc_64bit1k = 0x06,
};

enum page_type {
	pt_pagesize_512   = 0x00,
	pt_pagesize_2K    = 0x01,
	pt_pagesize_4K    = 0x02,
	pt_pagesize_8K    = 0x03,
	pt_pagesize_16K   = 0x04,
};

struct nand_config_info {
	unsigned int pagetype;
	unsigned int ecctype;
	unsigned int oobsize;
	struct nand_ecclayout *layout;
};

#endif

/*****************************************************************************/
struct nand_flash_dev_ex {
	struct nand_flash_dev flash_dev;

	char *start_type;
	unsigned char ids[8];
	int oobsize;
	int ecctype;

#define NAND_RANDOMIZER        0x01 /* nand chip need randomizer */
#define NAND_HW_AUTO           0x02 /*controller support hardware auto config*/
#define NAND_SYNCHRONOUS       0x04 /* nand chip support synchronous */
#define NAND_ASYNCHRONOUS      0x08 /* nand chip support asynchronous */
#define NAND_SYNCHRONOUS_BOOT  0x10 /* nand boot from synchronous mode */
#define NAND_CONFIG_DONE       0x20 /* current controller config finish */
	int flags;
	int is_randomizer;
#define NAND_RR_NONE                   0x00
#define NAND_RR_HYNIX_BG_BDIE          0x10
#define NAND_RR_HYNIX_BG_CDIE          0x11
#define NAND_RR_HYNIX_CG_ADIE          0x12
#define NAND_RR_MICRON                 0x20
#define NAND_RR_SAMSUNG                0x30
#define NAND_RR_TOSHIBA_24nm           0x40
#define NAND_RR_TOSHIBA_19nm           0x41
#define NAND_RR_MASK                   0xF0
	int read_retry_type;

	int hostver; /* host controller version. */
};

/*****************************************************************************/
int nandpage_size2type(int size);

int nandpage_type2size(int size);

/*****************************************************************************/
extern int nand_get_ecctype(void);

extern struct nand_flash_dev
	*(*nand_get_spl_flash_type)(struct mtd_info *mtd,
				    struct nand_chip *chip,
				    struct nand_flash_dev_ex *flash_dev_ex);

extern int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
			      struct nand_flash_dev_ex *flash_dev_ex);

#endif /* End of __CVSNFC_COMMON_H__ */

