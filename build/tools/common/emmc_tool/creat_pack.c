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
#include <libxml/parser.h>
#include "crc.h"

#define BM_PACK_HEADER_MAGIC	"bm_pack"
#define BM_PACK_MAX_ITEMS	(8)
#define BM_PACK_BLOCK_SIZE	(512)

struct bm_pack_item {
	char name[16];
	uint64_t offset;
	uint64_t size;
	uint8_t reserved[8];
	uint8_t md5[16];
};

struct bm_pack_header {
	char magic[15];
	uint8_t item_num;
	uint32_t blksz;
	struct bm_pack_item item[BM_PACK_MAX_ITEMS];
	uint8_t reserved[36];
	uint32_t hdr_crc;
};

struct bm_pack_header bm_header;
static const char *xml_path = ".";
static const char *output_file;

static void usage(void)
{
	printf("Usage:\n");
	printf("mk_package [options]\n\n"
		"options:\n"
		"  -p, --path   xml path\n"
		"  -o, --output   output file\n"
		"  -h, --help   display this help and exit\n");
	exit(EXIT_FAILURE);
}

static void parse_commandline(int argc, char **argv)
{
	int c;
	static struct option long_options[] = {
		{"path",  1, 0, 'p'},
		{"output",   1, 0, 'o'},
		{"help",  0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while (1) {
		c = getopt_long(argc, argv, "p:o:h", long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'p':
			xml_path = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'h':
		default:
			usage();
		}
	}
	if (output_file == NULL) {
		fprintf(stderr, "output file : NULL\n");
		usage();
	}
}

char *hexstr2bin (char* out_p, const char* input_p, const int in_len)
{
	int i=0;
	int tc;    
	for (i = 0; i < in_len ; i++)
	{
		sscanf(input_p+i*2, "%02X", (unsigned int*)(out_p+i));
	}
	return out_p;
}

#define MAX_BUF_SIZE	4096
static void write_item(int o_fd, xmlNodePtr xml_node, struct bm_pack_item *item)
{
	uint8_t buf[MAX_BUF_SIZE];
	xmlChar *prop = NULL;
	uint64_t size = 0;
	static int i = 0;
	int i_fd = -1, err = -1;

	prop = xmlGetProp(xml_node, BAD_CAST"name");
	if (prop == NULL) {
		fprintf(stderr, "name is NULL\n");
		exit(-1);
	}
	strncpy(item->name, prop, sizeof(item->name));
	xmlFree(prop);

	prop = xmlGetProp(xml_node, BAD_CAST"path");
	if (prop == NULL) {
		fprintf(stderr, "path is NULL\n");
		exit(-1);
	}

	i_fd = open(prop, O_RDWR);
	if (i_fd < 0) {
		fprintf(stderr, "open %s error\n", prop);
		exit(-1);
	}

	item->offset = lseek64(o_fd, 0, SEEK_CUR);
	while (1) {
		err = read(i_fd, buf, sizeof(buf));
		if (err < 0) {
			fprintf(stderr, "read %s error\n", prop);
			exit(-1);
		}
		size += err;

		if (err == 0) {
			break;
		}

		err = write(o_fd, buf, err);
		if (err < 0) {
			fprintf(stderr, "write %s error\n", prop);
			exit(-1);
		}

	}
	memset(buf, 0, sizeof(buf));
	write(o_fd, buf, BM_PACK_BLOCK_SIZE - (size & (BM_PACK_BLOCK_SIZE - 1)));
	item->size = size;
	xmlFree(prop);
	close(i_fd);

	prop = xmlGetProp(xml_node, BAD_CAST"md5");
	if (prop == NULL) {
		fprintf(stderr, "md5 is NULL\n");
		exit(-1);
	}
	printf("%s\n", prop);
	hexstr2bin(item->md5, prop, sizeof(item->md5));

	xmlFree(prop);

}

int main(int argc, char **argv)
{
	int ret = -1;
	int fd = -1;
	uint64_t size = 0;
	xmlDocPtr xml_doc = NULL;
	xmlNodePtr xml_node = NULL;
	xmlChar *prop = NULL;
	char file_name[PATH_MAX];
	parse_commandline(argc, argv);
	printf("xml_path : %s\n", xml_path);
	printf("output_file     : %s\n", output_file);
	fd = open(output_file, O_CREAT | O_TRUNC | O_RDWR | O_LARGEFILE, 0666);
	if (fd < 0) {
		fprintf(stderr, "open %s error\n", output_file);
		goto RETURN;
	}

	memset((void *)&bm_header, 0, sizeof(bm_header));
	strcpy(bm_header.magic, BM_PACK_HEADER_MAGIC);
	bm_header.blksz = BM_PACK_BLOCK_SIZE;
	if (bm_header.blksz < sizeof(bm_header)) {
		printf("block size must bigger than package header struct!!!\n");
		return -1;
	}
	lseek64(fd, bm_header.blksz, SEEK_SET);

	snprintf(file_name, PATH_MAX, "%s/package.xml", xml_path);
	xml_doc = xmlReadFile(file_name, "UTF-8", XML_PARSE_RECOVER);
	if (xml_doc == NULL) {
		fprintf(stderr, "xmlReadFile %s error\n", file_name);
		goto RETURN;
	}
	xml_node = xmlDocGetRootElement(xml_doc);
	while (xml_node != NULL) {
		if (xml_node->type == XML_ELEMENT_NODE)
			if (strcmp("header",
				   (char *)xml_node->name) == 0) {
				prop = xmlGetProp(xml_node, BAD_CAST"time");
				if (prop != NULL) {
					printf("[hq] create time:%s\n", prop);
					xmlFree(prop);
				}
				break;
			}
		xml_node = xml_node->next;
	}

	if (!xml_node) {
		fprintf(stderr, "find xml header failed\n");
		goto RETURN;
	}

	xml_node = xml_node->xmlChildrenNode;
	while (xml_node != NULL) {
		if (xml_node->type == XML_ELEMENT_NODE) {
			if (strcmp("item", (char *)xml_node->name) == 0) {
				write_item(fd, xml_node, &bm_header.item[bm_header.item_num]);
				bm_header.item_num++;
			}
			else
				printf("unknown node %s\n", xml_node->name);
		}
		xml_node = xml_node->next;
	}

	bm_header.hdr_crc = crc32(0, (uint8_t *)&bm_header, sizeof(bm_header) - sizeof(bm_header.hdr_crc));

	lseek64(fd, 0, SEEK_SET);
	write(fd, &bm_header, sizeof(bm_header));

	printf("\n");
	printf("header->magic:%s\n", bm_header.magic);
	printf("header->item_num:0x%x\n", bm_header.item_num);
	printf("header->hdr_crc32:0x%x\n", bm_header.hdr_crc);
	printf("\n");

	struct bm_pack_item *item_ptr = NULL;
	for (int i = 0; i < bm_header.item_num; ++i) {
		item_ptr = &bm_header.item[i];
		printf("\titem->name:%s\n", item_ptr->name);
		printf("\titem->size:0x%lx\n", item_ptr->size);
		printf("\titem->offset:0x%lx\n", item_ptr->offset);
		printf("\titem->md5:");
		for (int j = 0; j < sizeof(item_ptr->md5); ++j) {
			printf("%x", item_ptr->md5[j]);
		}
		printf("\n");
		printf("\n");
	}

RETURN:
	if (xml_doc != NULL)
		xmlFreeDoc(xml_doc);
	if (fd >= 0)
		close(fd);
	return ret;
}

