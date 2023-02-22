#ifndef __BL2_H__
#define __BL2_H__

#define BLOCK_SIZE 512

struct fip_param2 {
	uint64_t magic1;
	uint32_t param2_cksum;
	uint32_t reserved1;
	uint32_t ddr_param_cksum;
	uint32_t ddr_param_loadaddr;
	uint32_t ddr_param_size;
	uint32_t ddr_param_reserved;
	uint32_t blcp_2nd_cksum;
	uint32_t blcp_2nd_loadaddr;
	uint32_t blcp_2nd_size;
	uint32_t blcp_2nd_runaddr;
	uint32_t monitor_cksum;
	uint32_t monitor_loadaddr;
	uint32_t monitor_size;
	uint32_t monitor_runaddr;
	uint32_t loader_2nd_reserved0;
	uint32_t loader_2nd_loadaddr;
	uint32_t loader_2nd_reserved1;
	uint32_t loader_2nd_reserved2;
	uint8_t reserved4[4016];
} __packed;

struct loader_2nd_header {
	uint32_t jump0;
	uint32_t magic;
	uint32_t cksum;
	uint32_t size;
	uint64_t runaddr;
	uint32_t reserved1;
	uint32_t reserved2;
} __packed;

#define PARAM2_SIZE 0x1000

#define LOADER_2ND_MAGIC_RAW 0x20203342 // "B3  "
#define LOADER_2ND_MAGIC_LZMA 0x414d3342 // "B3MA"
#define LOADER_2ND_MAGIC_LZ4 0x345a3342 // "B3Z4"

#endif /* __BL2_H__ */
