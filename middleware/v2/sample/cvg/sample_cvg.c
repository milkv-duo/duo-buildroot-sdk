#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <poll.h>
#include <sys/wait.h>
#include "sample_cvg.h"
#include <sys/ioctl.h>

#define DEVICE_FILENAME		"/dev/cvi_gadget"

#define CVI_USB_S2D		0x81
#define CVI_USB_D2S		0x82

#define MMAP_SIZE		(2<<20)
#define BUF_SIZE		(1<<16)
#define OBJ_NUM			(MMAP_SIZE/BUF_SIZE)
#define CMD_SIZE		512
#define FILE_PATH_LEN		32
#define SIGCVG_CONN			44
#define SIGCVG_OUT			45
#define SIGCVG_IN			46

#define CVG_TEST_TYPE_PROTOCOL	0
#define CVG_TEST_TYPE_BLIND	1

#define CVG_MEM_TYPE_MMAP	0
#define CVG_MEM_TYPE_COPY	1
#define CVG_MEM_TYPE_ION	2

#define CVG_IO_TYPE_STREAM	0
#define CVG_IO_TYPE_BLK		1

#define CVG_ION_HEAP_ID		0  // carveout

struct cvg_cmd {
	int		dir;
	int		file_len;
	char		*file_path;
};

struct cvg_object {
	FILE		*fp;
	int		io_fd;
	int		ion_fd;
	int		map_fd;
	uint8_t		mem_type;
	uint8_t		io_type;
	uint8_t		test_type;
	uint8_t		reserve;
	int		queue_size;
	struct cvg_cmd	cmd;
};

struct cvg_stream_s {
	int		index;
	int		used;
	CVG_UURB_T	uurb;
	int (*cb)(struct cvg_stream_s *);
	int		remain;
	void		*context;
};

struct cvg_blind_header {
	int		len;
	uint16_t	chksum;
	uint16_t	reserve;
};

struct cvg_object cvg_obj;
struct cvg_stream_s cvg_stream[OBJ_NUM];
static int connect;

/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static const uint16_t crc16_tab[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
	0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
	0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
	0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
	0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
	0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
	0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
	0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
	0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
	0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
	0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

static int start_cpu_access(int write)
{
	int ret;
	int map_fd = cvg_obj.map_fd;
	struct dma_buf_sync sync;
	__u64 flags = (write?DMA_BUF_SYNC_WRITE:DMA_BUF_SYNC_READ) |
		DMA_BUF_SYNC_START;

	if (cvg_obj.mem_type != CVG_MEM_TYPE_ION)
		return 0;
	sync.flags = flags;
	ret = ioctl(map_fd, DMA_BUF_IOCTL_SYNC, &sync);
	if (ret < 0) {
		printf("start cpu access map_fd %d fail: %d: %s\n",
				map_fd, ret, strerror(errno));
		return -errno;
	}
	return ret;
}

static int stop_cpu_access(int write)
{
	int ret;
	int map_fd = cvg_obj.map_fd;
	struct dma_buf_sync sync;
	__u64 flags = (write?DMA_BUF_SYNC_WRITE:DMA_BUF_SYNC_READ) |
		DMA_BUF_SYNC_END;
	if (cvg_obj.mem_type != CVG_MEM_TYPE_ION)
		return 0;
	sync.flags = flags;
	ret = ioctl(map_fd, DMA_BUF_IOCTL_SYNC, &sync);
	if (ret < 0) {
		printf("stop cpu access map_fd %d fail: %d: %s\n",
				map_fd, ret, strerror(errno));
		return -errno;
	}
	return ret;
}

static uint16_t crc16_ccitt(uint8_t *buf, int len)
{
	uint16_t cksum = 0;

	for (int i = 0; i < len; i++) {
		cksum = crc16_tab[((cksum >> 8) ^ *buf++) & 0xff] ^ (cksum << 8);
	}

	return cksum;
}

static void fill_random_data(unsigned char *buf, int size)
{
	int i;
	time_t t;

	srand((unsigned int)time(&t));

	for (i = 0; i < size; i++)
		buf[i] = rand() & 0xFF;
}

/* 4 bytes data length, 2 bytes checksume.2 bytes padding. */
static int prepare_tx_data(unsigned char *buffer, int len)
{
	struct cvg_blind_header *header;
	int n = sizeof(*header);

	if (len < n)
		return -1;

	header = (struct cvg_blind_header *)buffer;
	if (len == n) {
		header->len = 0;
		header->chksum = 0;
		return 0;
	}
	fill_random_data(&buffer[n], len-n);
	header->len = len - sizeof(*header);
	header->chksum = crc16_ccitt(&buffer[n], len-n);

	//for (n=0; n<len; n++)
	//	printf("%d ", buffer[n]);
	//printf("\n");
	return 0;
}

static int parse_rx_data(unsigned char *buffer, int len)
{
	struct cvg_blind_header *header;
	int n = sizeof(*header);
	uint16_t chksum;

	if (len < n)
		return -1;

	header = (struct cvg_blind_header *)buffer;
	if (len == n) {
		if (!header->len && !header->chksum)
			return 0;
		else
			return -1;
	}
	if (header->len != (len - n)) {
		printf("len mismatch [%d, %d]\n", header->len, len-n);
		return -1;
	}
	chksum = crc16_ccitt(&buffer[n], len-n);
	if (chksum != header->chksum) {
		printf("checksum mismatch [%d, %d]\n", header->chksum, chksum);
		return -1;
	}
	return 0;
}

static int cvg_stream_compl(int io_fd, int dir, int timeout)
{
	struct pollfd event;
	CVG_UURB_T *uurb;
	struct cvg_stream_s *stream;
	int ret = 1, chunk;
	short poll_mask = (dir == CVG_UURB_DIR_IN) ? POLLIN : POLLOUT;

	event.fd = io_fd;
	event.events = poll_mask;
	event.events |= POLLERR;
	ret = poll(&event, sizeof(event)/sizeof(struct pollfd), timeout);
	if (!ret) {
		printf("timeout!\n");
		ret = -1;
		goto _exit;
	} else if (ret < 0) {
		printf("err return %d\n", ret);
		goto _exit;
	}
	if (event.revents & POLLERR) {
		printf("err event happens\n");
		ret = -1;
		goto _exit;
	}
	if (event.revents & poll_mask) {
		struct cvg_object *cvg;

		ret = ioctl(io_fd,
			    (event.revents&POLLOUT)?TOP_CVG_IOCTL_REAPTXURB:TOP_CVG_IOCTL_REAPRXURB,
			    (UINT_PTR_T)&uurb);
		syslog(LOG_DEBUG, "ret %d, status %d, actual %d\n", ret, uurb->status, uurb->actual_length);
		if (uurb->status) {
			printf("err:status: %d\n", uurb->status);
			ret = -1;
			goto _exit;
		}
		stream = (struct cvg_stream_s *)uurb->usercontext;
		cvg = (struct cvg_object *)stream->context;
		/* process data after stream in. */
		if (dir == CVG_UURB_DIR_IN) {
			if (stream->cb)
				stream->cb(stream);
		}
		stream->used = 0;
		/* no more pending request.*/
		syslog(LOG_DEBUG, "[%d] compl: remain %d, queue_size %d\n",
				stream->index,
				stream->remain,
				cvg->queue_size);
		if (!stream->remain) {
			ret = 0;
			goto _exit;
		}
		/* check if we need to submit another request. */
		if (stream->remain > cvg->queue_size) {
			chunk = ((stream->remain - cvg->queue_size) > BUF_SIZE)
				? BUF_SIZE : (stream->remain - cvg->queue_size);
			uurb->buffer_length = chunk;
			stream->remain = stream->remain - cvg->queue_size - chunk;
			stream->used = 1;
			/* prepare data before stream out. */
			if (dir == CVG_UURB_DIR_OUT) {
				if (stream->cb) {
					if (stream->cb(stream) < 0) {
						ret = -1;
						goto _exit;
					}
				}
			}
			ret = ioctl(io_fd, TOP_CVG_IOCTL_SUBMITURB, uurb);
			if (ret < 0) {
				syslog(LOG_ERR, "io submit urb fail %d\n", ret);
				ret = -1;
				goto _exit;
			}
			syslog(LOG_DEBUG, "[%d] compl: submit request %d, ret = %d\n",
					stream->index,
					stream->remain,
					ret);
		}
		ret = 1;
	}
_exit:
	return ret;
}

static int cvg_stream_io(int io_fd, uint64_t size,
		int timeout, int dir, int index, int depth,
		int (*cb)(struct cvg_stream_s *))
{
	int ret, chunk, remain = size, idx = index;

	if (io_fd < 0) {
		syslog(LOG_ERR, "wrong io fd!\n");
		ret = -1;
	}

	/* how many stream objects we need? */
	while (remain > 0) {
		CVG_UURB_T *uurb = &cvg_stream[idx++].uurb;
		struct cvg_stream_s *stream = (struct cvg_stream_s *)uurb->usercontext;

		chunk = remain > BUF_SIZE ? BUF_SIZE : remain;
		uurb->flags = dir;
		uurb->type = CVG_UURB_TYPE_BULK;
		uurb->buffer_length = chunk;
		stream->used = 1;
		stream->cb = cb;
		stream->remain = remain - chunk;
		/* prepare data before stream out. */
		if (dir == CVG_UURB_DIR_OUT) {
			if (stream->cb)
				if (cb(stream) < 0)
					return -1;
		}

		ret = ioctl(io_fd, TOP_CVG_IOCTL_SUBMITURB, uurb);
		if (ret < 0) {
			syslog(LOG_ERR, "io submit urb fail %d\n", ret);
			return -1;
		}

		remain -= chunk;
		if (idx == (index+depth))
			break;
	}
	while (cvg_stream_compl(io_fd, dir, timeout) > 0)
		;

	return ret;
}

static int cvg_io(int io_fd, unsigned char *buf, uint64_t size, int timeout, int dir)
{
	CVG_UURB_T uurb;
	CVG_UURB_T *result_uurb;
	struct pollfd event;
	short poll_mask = (dir == CVG_UURB_DIR_IN) ? POLLIN : POLLOUT;
	int ret;

	if (io_fd < 0) {
		syslog(LOG_ERR, "wrong io fd!\n");
		ret = -1;
	}

	memset(&uurb, 0, sizeof(uurb));
	uurb.buffer = (UINT_PTR_T)buf;
	uurb.flags = dir;
	uurb.type = CVG_UURB_TYPE_BULK;
	uurb.buffer_length = size;

	ret = ioctl(io_fd, TOP_CVG_IOCTL_SUBMITURB, &uurb);
	if (ret < 0) {
		printf("io submit urb fail %d\n", ret);
		return -1;
	}
	memset(&event, 0, sizeof(event));
	event.fd = io_fd;
	event.events = poll_mask;
	event.events |= POLLERR;
	ret = poll(&event, sizeof(event)/sizeof(struct pollfd), timeout);
	if (!ret) {
		printf("timeout!\n");
		ioctl(io_fd, TOP_CVG_IOCTL_DISCARDURB, &uurb);
		ret = -1;
	} else if (ret < 0) {
		printf("err return %d\n", ret);
	} else {
		if (event.revents & poll_mask) {
			ret = ioctl(io_fd,
				    (event.revents&POLLOUT)?TOP_CVG_IOCTL_REAPTXURB:TOP_CVG_IOCTL_REAPRXURB,
				     (UINT_PTR_T)&result_uurb);
			syslog(LOG_DEBUG, "ret %d, status %d, actual %d\n",
					ret,
					result_uurb->status,
					result_uurb->actual_length);
			syslog(LOG_DEBUG, "uurb: %p, result_uurb : %p\n", &uurb, result_uurb);
			if (result_uurb->status) {
				printf("err:status: %d\n", result_uurb->status);
				ret = -1;
			} else {
				return uurb.actual_length;
			}
		}
		if (event.revents & POLLERR) {
			syslog(LOG_ERR, "err event happens\n");
			ret = -1;
		}
	}

	return ret;
}

static int rcv_cmd(struct cvg_object *cvg, unsigned char *buf)
{
	ssize_t rd_sz = 0;

	cvg->cmd.file_path = (char *)malloc(FILE_PATH_LEN * sizeof(char));
	memset(cvg->cmd.file_path, 0, FILE_PATH_LEN);
	start_cpu_access(1);
	memset(buf, 0, CMD_SIZE);
	stop_cpu_access(1);
	rd_sz = cvg_io(cvg->io_fd, buf, CMD_SIZE, -1, CVG_UURB_DIR_IN);

	if (rd_sz < 0)
		return -1;
	start_cpu_access(0);
	/* parse command */
	syslog(LOG_DEBUG, "rd_sz: %zd\n", rd_sz);
	syslog(LOG_DEBUG, "\n============\n");
	cvg->cmd.dir = buf[0];
	cvg->cmd.file_len = (((uint64_t)(buf[1]) << 32) +
		((uint64_t)(buf[2]) << 24) +
		((uint64_t)(buf[3]) << 16) +
		((uint64_t)(buf[4]) << 8) +
		((uint64_t)(buf[5])));
	memcpy(cvg->cmd.file_path, &buf[6], rd_sz - 6);
	stop_cpu_access(0);
	syslog(LOG_DEBUG, "req len:%d\n", cvg->cmd.file_len);
	syslog(LOG_DEBUG, "file path : %s\n", cvg->cmd.file_path);
	return 0;
}

static int load_send_data(struct cvg_object *cvg, unsigned char *buf)
{
	size_t total = cvg->cmd.file_len;
	ssize_t sd_sz = 0, req_sz;

	cvg->fp = fopen(cvg->cmd.file_path, "rb");

	if (cvg->fp == NULL) {
		syslog(LOG_ERR, "open fail\n");
		return -1;
	}

	while (total > 0) {
		if (total > BUF_SIZE)
			req_sz = BUF_SIZE;
		else
			req_sz = total;
		start_cpu_access(1);
		sd_sz = fread(buf, 1, req_sz, cvg->fp);
		stop_cpu_access(1);
		syslog(LOG_DEBUG, "fread %zd\n", sd_sz);
		if (sd_sz != req_sz) {
			syslog(LOG_ERR, "f_read fail %d\n", errno);
			fclose(cvg->fp);
			return -1;
		}
		if (!sd_sz) {
			syslog(LOG_DEBUG, "f_read no more data\n");
			break;
		}

		if (cvg_io(cvg->io_fd, buf, sd_sz, 5000, CVG_UURB_DIR_OUT) < 0) {
			syslog(LOG_ERR, "usb write fail!!\n");
			fclose(cvg->fp);
			return -1;
		}
		total -= sd_sz;
		syslog(LOG_DEBUG, "rd_sz: %zu\n", sd_sz);
		syslog(LOG_DEBUG, "ttl_rd_size : %zu\n", total);
	}
	fclose(cvg->fp);

	return 0;

}

static int rcv_store_data(struct cvg_object *cvg, unsigned char *buf)
{
	size_t total = cvg->cmd.file_len;
	ssize_t rd_sz = 0, req_sz = 0;

	cvg->fp = fopen(cvg->cmd.file_path, "wb");

	if (cvg->fp == NULL) {
		syslog(LOG_ERR, "open fail\n");
		return -1;
	}

	while (total > 0) {
		if (total > BUF_SIZE)
			req_sz = BUF_SIZE;
		else
			req_sz = total;
		rd_sz = cvg_io(cvg->io_fd, buf, req_sz, 5000, CVG_UURB_DIR_IN);
		if (rd_sz < 0) {
			syslog(LOG_ERR, "read fail!!\n");
			return -1;
		}
		syslog(LOG_DEBUG, "rd_sz: %zu\n", rd_sz);
		total -= rd_sz;
		syslog(LOG_DEBUG, "ttl_rd_size : %zu\n", total);
		start_cpu_access(0);
		fwrite(buf, rd_sz, 1, cvg->fp);
		stop_cpu_access(0);
	}
	fclose(cvg->fp);

	return 0;
}
static int prepare_stream_out(struct cvg_stream_s *stream)
{
	struct cvg_object *cvg = (struct cvg_object *)stream->context;
	FILE *fp = cvg->fp;
	CVG_UURB_T *uurb = &stream->uurb;
	int size;

	start_cpu_access(0);
	size = fread((void *)uurb->buffer, 1, uurb->buffer_length, fp);
	stop_cpu_access(0);
	if (size != uurb->buffer_length) {
		syslog(LOG_ERR, "f_read fail %d\n", errno);
		return -1;
	}
	syslog(LOG_DEBUG, "prepare stream [%d] %dB\n", stream->index, uurb->buffer_length);
	return 0;
}

static int proc_stream_in(struct cvg_stream_s *stream)
{
	struct cvg_object *cvg = (struct cvg_object *)stream->context;
	FILE *fp = cvg->fp;
	CVG_UURB_T *uurb = &stream->uurb;

	start_cpu_access(1);
	fwrite((void *)uurb->buffer, 1, uurb->actual_length, fp);
	stop_cpu_access(1);

	syslog(LOG_DEBUG, "handle stream [%d] %dB\n", stream->index, uurb->actual_length);
	return 0;
}

static int stream_rcv_data(struct cvg_object *cvg)
{
	int ret;

	cvg->fp = fopen(cvg->cmd.file_path, "wb");
	if (cvg->fp == NULL) {
		syslog(LOG_ERR, "open fail\n");
		return -1;
	}
	ret = cvg_stream_io(cvg->io_fd, cvg->cmd.file_len, -1,
			    CVG_UURB_DIR_IN, 0, OBJ_NUM, proc_stream_in);

	fclose(cvg->fp);

	return ret;
}

static int blind_rcv(struct cvg_object *cvg, unsigned char *buf, int length)
{
	int ret;

	cvg_stream_io(cvg->io_fd, length, -1,
			    CVG_UURB_DIR_IN, 0, OBJ_NUM>>1, NULL);
	ret = parse_rx_data(buf, length);

	if (ret < 0)
		printf("parse crc fail at %d\n", length);

	return ret;
}

static int stream_send_data(struct cvg_object *cvg)
{
	int ret;

	cvg->fp = fopen(cvg->cmd.file_path, "rb");
	if (cvg->fp == NULL) {
		syslog(LOG_ERR, "open fail\n");
		return -1;
	}
	ret = cvg_stream_io(cvg->io_fd, cvg->cmd.file_len, -1,
			    CVG_UURB_DIR_OUT,  0, OBJ_NUM, prepare_stream_out);

	fclose(cvg->fp);

	return ret;

}

static int blind_snd(struct cvg_object *cvg, unsigned char *buf, int length)
{
	int ret;

	/* limit the resource to half of the mmap. */
	ret = prepare_tx_data(buf, length);
	if (ret >= 0)
		ret = cvg_stream_io(cvg->io_fd, length, -1,
				    CVG_UURB_DIR_OUT, OBJ_NUM>>1, OBJ_NUM>>1, NULL);

	return ret;
}

static int register_connect_ioctl(int io_fd)
{
	CVG_CONNECTSIGNAL_T cs;
	int ret;

	memset(&cs, 0, sizeof(cs));
	cs.signr = SIGCVG_CONN;
	ret = ioctl(io_fd, TOP_CVG_IOCTL_CONNSIG, &cs);
	if (ret < 0) {
		printf("register connect signal fail %d", ret);
		return -1;
	}
	return 0;
}

static int proc_protocol_transfer(struct cvg_object *cvg, unsigned char *buffer)
{
	int ret;

	/* receive commands. */
	if (rcv_cmd(cvg, buffer) < 0)
		return -1;
	if (cvg->cmd.dir == CVI_USB_D2S) {
		/* load then send data. */
		if (cvg->io_type == CVG_IO_TYPE_BLK) {
			ret = load_send_data(cvg, buffer);
		} else if (cvg->io_type == CVG_IO_TYPE_STREAM) {
			ret = stream_send_data(cvg);
		} else {
			printf("Unsupport io_type %d\n", cvg->io_type);
			ret = -1;
		}
	} else if (cvg->cmd.dir == CVI_USB_S2D) {
		/* recevie then store data. */
		if (cvg->io_type == CVG_IO_TYPE_BLK) {
			ret = rcv_store_data(cvg, buffer);
		} else if (cvg->io_type == CVG_IO_TYPE_STREAM) {
			ret = stream_rcv_data(cvg);
		} else {
			printf("Unsupport io_type %d\n", cvg->io_type);
			ret = -1;
		}
	} else {
		ret = -1;
	}
	return ret;
}

static int proc_blind_tx(struct cvg_object *cvg, unsigned char *buffer)
{
	int ret;
	int offset = 8;
	int start_len = sizeof(struct cvg_blind_header) + 4;
	int req_len = start_len;

	while (connect) {
		ret = blind_snd(cvg, buffer, req_len);
		if (ret < 0)
			exit(EXIT_FAILURE);
		req_len += offset;
		/* warp around if over the maximum buffer size. */
		if (req_len > (MMAP_SIZE>>1))
			req_len = start_len;
	}

	exit(EXIT_SUCCESS);
}

static int proc_blind_rx(struct cvg_object *cvg, unsigned char *buffer)
{
	int ret;
	int offset = 8;
	int start_len = sizeof(struct cvg_blind_header) + 4;
	static int req_len = sizeof(struct cvg_blind_header) + 4;

	ret = blind_rcv(cvg, buffer, req_len);
	req_len += offset;
	/* warp around if over the maximum buffer size. */
	if (req_len > (MMAP_SIZE>>1))
		req_len = start_len;

	return ret;
}

static int proc_transfer(struct cvg_object *cvg, unsigned char *buffer)
{
	int ret = -1;
	sigset_t mask;
	pid_t child;

	printf("proc_transfer start: io_type = %d\n", cvg->io_type);
	/* BLOCK SIGCVG_CONN signal. */
	sigemptyset(&mask);
	sigaddset(&mask, SIGCVG_OUT);
	sigaddset(&mask, SIGCVG_IN);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		printf("Cannot block INOUT signal: %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (cvg->test_type == CVG_TEST_TYPE_BLIND) {
		child = fork();
		if (child == -1) {
			printf("Cannot fork a tx proc: %s.\n", strerror(errno));
		} else {
			if (!child) {
				/* TX uses the bottom-half memory*/
				return proc_blind_tx(cvg, buffer+(MMAP_SIZE>>1));
			}
		}
	}
	while (connect) {
		if (cvg->test_type == CVG_TEST_TYPE_PROTOCOL) {
			ret = proc_protocol_transfer(cvg, buffer);
		} else if (cvg->test_type == CVG_TEST_TYPE_BLIND) {
			/* RX uses the top-half memory*/
			ret = proc_blind_rx(cvg, buffer);
		} else {
			ret = -1;
		}
		if (ret < 0)
			break;
	}
	printf("proc_transfer exit with %s\n", ret < 0?"fail":"success");
	exit((ret < 0) ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void query_ion_heap(int fd)
{
	struct ion_heap_data heap_data[6];
	struct ion_heap_query query;
	int ret;
	unsigned int i;

	memset(heap_data, 0, sizeof(heap_data));
	query.cnt = 6;
	query.heaps = (size_t)heap_data;
	query.reserved0 = 0;
	query.reserved1 = 0;
	query.reserved2 = 0;
	ret = ioctl(fd, ION_IOC_HEAP_QUERY, &query);
	if (ret < 0) {
		printf("%s: failed\n", __func__);
		return;
	}
	for (i = 0; i < query.cnt; i++) {
		struct ion_heap_data *heap = &heap_data[i];

		printf("heap name = %s, type = %d, id = %d\n",
				heap->name,
				heap->type,
				heap->heap_id);
	}
}

static unsigned char *ion_alloc(int fd, __u64 size, int dev_fd, __u32 heap_id, int *map_fd)
{
	int ret;
	struct ion_allocation_data data = {
		.len = size,
		.heap_id_mask = 1u << heap_id,
		.flags = 0,
		.fd = 0,
		.unused = 0,
		.paddr = 0,
	};
	unsigned char *ptr;
	CVG_ION_QUEUE_T q;

	ret = ioctl(fd, ION_IOC_ALLOC, &data);
	if (ret < 0) {
		printf("%s failed: %s\n", __func__, strerror(ret));
		return NULL;
	}
	if (!data.fd) {
		printf("%s failed: cannot get dma_buf fd %s\n", __func__, strerror(ret));
		return NULL;
	}
	*map_fd = data.fd;
	q.fd = data.fd;
	ptr = (unsigned char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, data.fd, 0);
	if (ptr == MAP_FAILED) {
		printf("map_fd = %d mmap failed: %s\n", data.fd, strerror(errno));
		return NULL;
	}
	q.vbase = (UINT_PTR_T)ptr;
	ret = ioctl(dev_fd, TOP_CVG_IOCTL_QUEUEION, &q);
	if (ret) {
		printf("%s failed, cannot queue ion buf, %s\n", __func__, strerror(ret));
		return NULL;
	}

	return ptr;
}

static int wait_child_proc(pid_t child, int *status)
{
	pid_t p = 0xFFFFFFFF;

	if (child > 0) {
		do {
			p = waitpid(child, status, 0);
			if (p == -1) {
				if (errno == EINTR)
					continue;
				printf("Parent: waitpid error %s.\n",
					strerror(errno));
				return -1;
			}
		} while (p != child);

		if (WIFEXITED(*status) &&
		    (WEXITSTATUS(*status) == EXIT_FAILURE))
			return -1;
	}

	return 0;
}

int test_io(int test_type, int io_type, int mem_type)
{
	struct cvg_object *cvg = &cvg_obj;
	int i, ret, status;
	unsigned char *buffer;
	siginfo_t info;
	sigset_t mask;
	pid_t child = 0xFFFFFFFF;
	int ion_fd, map_fd = 0;

	cvg->io_fd = open(DEVICE_FILENAME, O_RDWR);
	syslog(LOG_DEBUG, "fd = %d\n", cvg->io_fd);
	if (cvg->io_fd <= 0) {
		printf("dev not ready\n");
		return EXIT_FAILURE;
	}

	cvg->test_type = test_type;
	cvg->mem_type = mem_type;
	cvg->io_type = io_type;

	switch (mem_type) {
	case CVG_MEM_TYPE_COPY:
		/* allocate 4MB buffer. */
		buffer = (unsigned char *)malloc(MMAP_SIZE);
		break;
	case CVG_MEM_TYPE_MMAP:
		/* get 4MB buffer from kernel. */
		buffer = (unsigned char *)mmap(NULL, MMAP_SIZE,
			PROT_READ | PROT_WRITE, MAP_SHARED, cvg->io_fd, 0);
		break;
	case CVG_MEM_TYPE_ION:
		/* get 4MB buffer from ION. */
		printf("open ion node\n");
		ion_fd = open("/dev/ion", O_RDWR);
		if (ion_fd < 0) {
			printf("open /dev/ion failed!\n");
			return -errno;
		}
		cvg->ion_fd = ion_fd;
		query_ion_heap(ion_fd);
		buffer = ion_alloc(ion_fd, MMAP_SIZE, cvg->io_fd, CVG_ION_HEAP_ID, &map_fd);
		cvg->map_fd = map_fd;
		printf("alloc dev mem 4 %p\n", buffer);
		break;
	default:
		printf("Unsupport memory type %d\n", mem_type);
		exit(EXIT_FAILURE);
	}
	if (!buffer) {
		syslog(LOG_ERR, "alloc dev mem failed errno %d\n", errno);
		return EXIT_FAILURE;
	}

	syslog(LOG_DEBUG, "alloc dev mem 4 %p\n", buffer);

	if (io_type == CVG_IO_TYPE_STREAM) {
		for (i = 0; i < OBJ_NUM; i++) {
			CVG_UURB_T *uurb = &cvg_stream[i].uurb;

			cvg_stream[i].index = i;
			cvg_stream[i].used = 0;
			cvg_stream[i].context = (void *)cvg;
			uurb->buffer = (UINT_PTR_T)(buffer+i*BUF_SIZE);
			uurb->buffer_length = BUF_SIZE;
			uurb->usercontext = (UINT_PTR_T)&cvg_stream[i];
		}
	}

	/* Separate the resource for rx/tx blind test. */
	if (cvg->test_type == CVG_TEST_TYPE_BLIND)
		cvg->queue_size = BUF_SIZE * ((OBJ_NUM>>1) - 1);
	else
		cvg->queue_size = BUF_SIZE * (OBJ_NUM - 1);

	/* BLOCK SIGCVG_CONN signal. */
	sigemptyset(&mask);
	sigaddset(&mask, SIGCVG_CONN);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		printf("Cannot block SIGCVG_CONN: %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}

	/* Register the signr to cvg. The signal would be fired
	 * immediately if the connect is true. So we need to block
	 * the ISGCVG_CONN first.
	 */
	register_connect_ioctl(cvg->io_fd);

_reconn:
	sigemptyset(&mask);
	sigaddset(&mask, SIGCVG_CONN);
	ret = sigwaitinfo(&mask, &info);
	if (ret == -1) {
		if (errno == EINTR)
			goto _reconn;
		printf("Parent process: sigwaitinfo() failed: %s.\n", strerror(errno));
		goto _exit;
	}
	if (info.si_signo == SIGCVG_CONN) {
		connect = info.si_errno ? 0 : 1;
		printf("function %s\n", connect?"connect":"disconnect");
		if (!connect) {
			/* wait transfer process terminated. */
			if (wait_child_proc(child, &status) < 0)
				goto _exit;
		} else {
			/* create transfer child process. */
			child = fork();
			if (child == -1) {
				printf("Cannot fork a event handler: %s.\n", strerror(errno));
				goto _exit;
			} else {
				if (!child) {
					return proc_transfer(cvg, buffer);
				}
			}
		}
		goto _reconn;
	} else {
		printf("other signal number %d, %d\n", ret, info.si_signo);
	}

_exit:
	close(cvg->io_fd);

	return EXIT_SUCCESS;
}

void print_usage(void)
{
	printf("cvg_test [testcase]:\n");
	printf("[testcase]: 0 blocking read/write, mmap\n");
	printf("            1 blocking read/write, mem copy\n");
	printf("            2 streaming read/write, mmap\n");
	printf("            3 streaming read/write, mem copy\n");
	printf("            4 blind test\n");
	printf("            5 blocking read/write, ion\n");
	printf("            6 streaming read/write, ion\n");
}

int main(int argc, char **argv)
{
	int testcase;

	if (argc != 2) {
		print_usage();
		return -1;
	}
	testcase = atoi(argv[1]);

	switch (testcase) {
	case 0:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_BLK, CVG_MEM_TYPE_MMAP);
		break;
	case 1:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_BLK, CVG_MEM_TYPE_COPY);
		break;
	case 2:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_STREAM, CVG_MEM_TYPE_MMAP);
		break;
	case 3:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_STREAM, CVG_MEM_TYPE_COPY);
		break;
	case 4:
		test_io(CVG_TEST_TYPE_BLIND, CVG_IO_TYPE_STREAM, CVG_MEM_TYPE_MMAP);
		break;
	case 5:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_BLK, CVG_MEM_TYPE_ION);
		break;
	case 6:
		test_io(CVG_TEST_TYPE_PROTOCOL, CVG_IO_TYPE_STREAM, CVG_MEM_TYPE_ION);
		break;
	default:
		print_usage();
		break;
	}
	return 0;
}
