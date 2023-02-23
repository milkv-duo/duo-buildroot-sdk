#ifndef _BM_IMAGE_H_
#define _BM_IMAGE_H_

#include "stdint.h"

#define MAX_LABEL_SIZE                 (8)
#define MAX_FILENAME_SIZE              (16)

struct guid_t {
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint8_t clock_seq_hi_and_reserved;
	uint8_t clock_seq_low;
	uint8_t node[6];
} __attribute__((packed));

#define LBA_SIZE                       (512)
#define GPT_HEADER_SIZE                (92)
#define ENTRY_NUM                      (128)
#define ENTRY_SIZE                     (128)
#define ENTRY_LBA                      (ENTRY_NUM * ENTRY_SIZE / LBA_SIZE)

struct partition_record_t {
	uint8_t boot_indicator;
	uint8_t start_head;
	uint8_t start_sector;
	uint8_t start_track;
	uint8_t os_type;
	uint8_t end_head;
	uint8_t end_sector;
	uint8_t end_track;
	uint32_t starting_lba;
	uint32_t size_in_lba;
} __attribute__((packed));

struct legacy_mbr_t {
	uint8_t boot_code[440];
	uint32_t unique_mbr_signature;
	uint16_t unknown;
	struct partition_record_t part_record[4];
	uint16_t signature;
} __attribute__((packed));

struct gpt_header_t {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t header_crc32;
	uint32_t reserved1;
	uint64_t my_lba;
	uint64_t alternate_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	struct guid_t disk_guid;
	uint64_t part_entry_lba;
	uint32_t number_of_entries;
	uint32_t size_of_entry;
	uint32_t entry_array_crc32;
	uint8_t reserved2[LBA_SIZE - GPT_HEADER_SIZE];
} __attribute__((packed));

struct gpt_entry_t {
	struct guid_t part_type_guid;
	struct guid_t unique_part_guid;
	uint64_t starting_lba;
	uint64_t ending_lba;
	uint64_t attributes;
	uint16_t part_name[(ENTRY_SIZE - 56) / 2];
} __attribute__((packed));

#define MBR_SIGNATURE                  (0xAA55)
#define MBR_OSTYPE_EFI                 (0xEE)
#define GPT_HEADER_SIGNATURE           (0x5452415020494645)
#define GPT_REVISION                   (0x00010000)

#define FIND_GPT_START_BLOCK           1
#define FIND_GPT_END_BLOCK             8

#endif  /* _BM_IMAGE_H_ */
