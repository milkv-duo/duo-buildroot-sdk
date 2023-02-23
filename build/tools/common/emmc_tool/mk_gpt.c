#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <getopt.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include "uuid.h"
#include <libxml/parser.h>
#include "crc.h"
#include "mk_gpt.h"

static const char *xml_file = ".";
static const char *update_dev;
static struct legacy_mbr_t mbr;
static struct gpt_header_t pheader;
static struct gpt_entry_t entries[ENTRY_NUM];
static struct gpt_header_t bheader;
static uint32_t current_part;
static uint64_t current_lba;

static void usage(void)
{
	printf("Usage:\n");
	printf("mk_gpt [options]\n\n"
		"options:\n"
		"  -p, --path   xml path\n"
		"  -d, --dev    dev or output file\n"
		"  -h, --help   display this help and exit\n");
	exit(EXIT_FAILURE);
}

static void parse_commandline(int argc, char **argv)
{
	int c;
	static struct option long_options[] = {
		{"path",  1, 0, 'p'},
		{"dev",   1, 0, 'd'},
		{"help",  0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while (1) {
		c = getopt_long(argc, argv, "p:d:h", long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'p':
			xml_file = optarg;
			break;
		case 'd':
			update_dev = optarg;
			break;
		case 'h':
		default:
			usage();
		}
	}
	if (update_dev == NULL) {
		fprintf(stderr, "sd dev : NULL\n");
		usage();
	}
}

static void uuid2guid(struct guid_t *guid, uuid_t uuid)
{
	guid->time_low = (((uint32_t)uuid[0] & 0xFF) << 24) |
			 (((uint32_t)uuid[1] & 0xFF) << 16) |
			 (((uint32_t)uuid[2] & 0xFF) << 8) |
			 (((uint32_t)uuid[3] & 0xFF) << 0);
	guid->time_mid = (((uint16_t)uuid[4] & 0xFF) << 8) |
			 (((uint16_t)uuid[5] & 0xFF) << 0);
	guid->time_hi_and_version = (((uint16_t)uuid[6] & 0xFF) << 8) |
				    (((uint16_t)uuid[7] & 0xFF) << 0);
	guid->clock_seq_hi_and_reserved = (uint8_t)uuid[8];
	guid->clock_seq_low = (uint8_t)uuid[9];
	memcpy(guid->node, &uuid[10], 6);
}

static void init_gpt(uint64_t total_size)
{
	uint64_t sector = total_size / LBA_SIZE;
	uuid_t uuid;
	memset(&mbr, 0, sizeof(mbr));
	memset(&pheader, 0, sizeof(pheader));
	memset(&entries, 0, sizeof(entries));
	memset(&pheader, 0, sizeof(pheader));
	mbr.signature = MBR_SIGNATURE;
	mbr.part_record[0].os_type = MBR_OSTYPE_EFI;
	mbr.part_record[0].start_sector = 1;
	mbr.part_record[0].end_head = 0xFE;
	mbr.part_record[0].end_sector = 0xFF;
	mbr.part_record[0].end_track = 0xFF;
	mbr.part_record[0].starting_lba = 1;
	if (sector > 0xFFFFFFFF)
		mbr.part_record[0].size_in_lba = 0xFFFFFFFF;
	else
		mbr.part_record[0].size_in_lba = sector - 1;
	pheader.signature = GPT_HEADER_SIGNATURE;
	bheader.signature = GPT_HEADER_SIGNATURE;
	pheader.revision = GPT_REVISION;
	bheader.revision = GPT_REVISION;
	pheader.header_size = GPT_HEADER_SIZE;
	bheader.header_size = GPT_HEADER_SIZE;
	pheader.my_lba = 1;
	bheader.my_lba = sector - 1;
	pheader.alternate_lba = sector - 1;
	bheader.alternate_lba = 1;
	pheader.part_entry_lba = 2;
	bheader.part_entry_lba = sector - (ENTRY_LBA + 1);
	pheader.first_usable_lba = 2 + ENTRY_LBA;
	bheader.first_usable_lba = 2 + ENTRY_LBA;
	pheader.last_usable_lba = sector - (ENTRY_LBA + 2);
	bheader.last_usable_lba = sector - (ENTRY_LBA + 2);
	uuid_generate(uuid);
	uuid2guid(&pheader.disk_guid, uuid);
	uuid2guid(&bheader.disk_guid, uuid);
	pheader.number_of_entries = ENTRY_NUM;
	bheader.number_of_entries = ENTRY_NUM;
	pheader.size_of_entry = ENTRY_SIZE;
	bheader.size_of_entry = ENTRY_SIZE;
	current_part = 0;
	current_lba = 8192;
}

static void write_partition(int fd, xmlNodePtr xml_node)
{
	//PARTITION_BASIC_DATA_GUID
	const struct guid_t type = { 0xEBD0A0A2, 0xB9E5, 0x4433, 0x87, 0xC0,
				     { 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7 } };
	xmlChar *prop = NULL;
	uint64_t size = 0;
	int sparse = 0;
	int readonly = 0;
	uint64_t start = current_lba;
	uint64_t end;
	uuid_t uuid;
	int i;
	prop = xmlGetProp(xml_node, BAD_CAST"size_in_kb");
	if (prop != NULL) {
		size = (uint64_t)atoll((char *)prop) * 1024;
		xmlFree(prop);
	}
	prop = xmlGetProp(xml_node, BAD_CAST"readonly");
	if (prop != NULL) {
		if (strcmp("true", (char *)prop) == 0)
			readonly = 1;
		else if (strcmp("false", (char *)prop) == 0)
			readonly = 0;
		else
			printf("readonly attribute error, use default 0\n");
		xmlFree(prop);
	}
	end = start + ((size + LBA_SIZE - 1) / LBA_SIZE) - 1;
	memcpy(&entries[current_part].part_type_guid, &type, 16);
	uuid_generate(uuid);
	uuid2guid(&entries[current_part].unique_part_guid, uuid);
	entries[current_part].starting_lba = start;
	entries[current_part].ending_lba = end;
	if (readonly) {
		entries[current_part].attributes = (uint64_t)1 << 60;
	}
	prop = xmlGetProp(xml_node, BAD_CAST"label");
	if (prop != NULL) {
		for (i = 0; i < (ENTRY_SIZE - 56) / 2; i++) {
			entries[current_part].part_name[i] = (uint16_t)prop[i];
			if (entries[current_part].part_name[i] == 0)
				break;
		}
		xmlFree(prop);
	}
	current_part++;
	current_lba = end + 1;
}

static void save_gpt(int fd)
{
	uint32_t crc = crc32(0xFFFFFFFF, (uint8_t *)&entries,
			     ENTRY_NUM * ENTRY_SIZE);
	crc ^= 0xFFFFFFFF;
	pheader.entry_array_crc32 = crc;
	bheader.entry_array_crc32 = crc;
	pheader.header_crc32 = 0;
	crc = crc32(0xFFFFFFFF, (uint8_t *)&pheader, GPT_HEADER_SIZE);
	crc ^= 0xFFFFFFFF;
	pheader.header_crc32 = crc;
	bheader.header_crc32 = 0;
	crc = crc32(0xFFFFFFFF, (uint8_t *)&bheader, GPT_HEADER_SIZE);
	crc ^= 0xFFFFFFFF;
	bheader.header_crc32 = crc;
	lseek64(fd, 0, SEEK_SET);
	write(fd, &mbr, sizeof(mbr));
	lseek64(fd, pheader.my_lba * LBA_SIZE, SEEK_SET);
	write(fd, &pheader, sizeof(pheader));
	lseek64(fd, pheader.part_entry_lba * LBA_SIZE, SEEK_SET);
	write(fd, &entries, sizeof(entries));
#if 0
	lseek64(fd, bheader.my_lba * LBA_SIZE, SEEK_SET);
	write(fd, &bheader, sizeof(bheader));
	lseek64(fd, bheader.part_entry_lba * LBA_SIZE, SEEK_SET);
	write(fd, &entries, sizeof(entries));
#endif
}

int main(int argc, char **argv)
{
	int ret = 0;
	int fd = -1;
	uint64_t size = 0;
	xmlDocPtr xml_doc = NULL;
	xmlNodePtr xml_node = NULL;
	xmlChar *prop = NULL;
	char file_name[PATH_MAX];
	parse_commandline(argc, argv);
	printf("xml_file : %s\n", xml_file);
	printf("update_dev     : %s\n", update_dev);
	fd = open(update_dev, O_CREAT | O_TRUNC | O_RDWR | O_LARGEFILE, 0666);
	if (fd < 0) {
		fprintf(stderr, "open %s error\n", update_dev);
		goto RETURN;
	}
	
	snprintf(file_name, PATH_MAX, "%s", xml_file);
	xml_doc = xmlReadFile(file_name, "UTF-8", XML_PARSE_RECOVER);
	if (xml_doc == NULL) {
		fprintf(stderr, "xmlReadFile %s error\n", file_name);
		goto RETURN;
	}
	xml_node = xmlDocGetRootElement(xml_doc);
	while (xml_node != NULL) {
		if (xml_node->type == XML_ELEMENT_NODE)
			if (strcmp("physical_partition",
				   (char *)xml_node->name) == 0) {
				prop = xmlGetProp(xml_node, BAD_CAST"size_in_kb");
				if (prop != NULL) {
					size = (uint64_t)atoll((char *)prop) * 1024;
					printf("[hq] total size:%ld\n", size);
					xmlFree(prop);
				}
				break;
			}
		xml_node = xml_node->next;
	}

	if (!xml_node) {
		fprintf(stderr, "find physical_partition failed\n");
		goto RETURN;
	}

	init_gpt(size);

	if (xml_node == NULL) {
		fprintf(stderr, "find physical_partition error\n");
		goto RETURN;
	}
	xml_node = xml_node->xmlChildrenNode;
	while (xml_node != NULL) {
		if (xml_node->type == XML_ELEMENT_NODE) {
			if (strcmp("partition", (char *)xml_node->name) == 0)
				write_partition(fd, xml_node);
			else
				printf("unknown node %s\n", xml_node->name);
		}
		xml_node = xml_node->next;
	}
	save_gpt(fd);
RETURN:
	if (xml_doc != NULL)
		xmlFreeDoc(xml_doc);
	if (fd >= 0)
		close(fd);
	return ret;
}

