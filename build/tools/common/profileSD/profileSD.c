#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>

#define KB(x) (x<<10)
#define MB(x) (x<<20)
#define GB(x) (x<<30)
#define FIX_DATA_SIZE KB(512) // 512KB each time
#define RAND_NUM_DATA_SIZE (21)

char mode[5];
char op_mode[8];
int file_op = -1;
char file_path[128];
int repeat_times;
size_t stImageSize;
int fd = 0;
FILE *fp = NULL;

typedef struct _stRandBuf {
	char *buf;
	size_t buf_size;
} stRandBuf;

enum _eFileOpMode {
	SYSTEM_CALL = 0,
	FILE_STREAM,
};

static stRandBuf _gstRandBuf[RAND_NUM_DATA_SIZE] = {
	{.buf = NULL, .buf_size = 29}, {.buf = NULL, .buf_size = 30290}, {.buf = NULL, .buf_size = 3435},
	{.buf = NULL, .buf_size = 235}, {.buf = NULL, .buf_size = 12345}, {.buf = NULL, .buf_size = 80},
	{.buf = NULL, .buf_size = 9845}, {.buf = NULL, .buf_size = 564}, {.buf = NULL, .buf_size = 34862},
	{.buf = NULL, .buf_size = 123}, {.buf = NULL, .buf_size = 267890}, {.buf = NULL, .buf_size = 36},
	{.buf = NULL, .buf_size = 6788}, {.buf = NULL, .buf_size = 86}, {.buf = NULL, .buf_size = 234567},
	{.buf = NULL, .buf_size = 1232}, {.buf = NULL, .buf_size = 514}, {.buf = NULL, .buf_size = 50},
	{.buf = NULL, .buf_size = 678}, {.buf = NULL, .buf_size = 9864}, {.buf = NULL, .buf_size = 333333},
};

static void inline drop_cache(void)
{
	system("echo 3 > /proc/sys/vm/drop_caches");
}

static double MeasureFIX_WriteSpeed(char *buf, size_t buf_size, size_t total)
{
	struct timeval start, end;
	double Speed_in_MB = 0, time_in_s = 0;
	size_t cur_size = 0;

	memset(buf, 0x1F, buf_size);

	if (file_op == SYSTEM_CALL) {
		lseek(fd, 0x0, SEEK_SET);
	} else if (file_op == FILE_STREAM) {
		fseek(fp, 0x0, SEEK_SET);
	}
	drop_cache();

	if (file_op == SYSTEM_CALL) {
		gettimeofday(&start, NULL);
		do {
			cur_size += write(fd, buf, buf_size);
		} while( cur_size < total);
		gettimeofday(&end, NULL);
	} else if (file_op == FILE_STREAM) {
		gettimeofday(&start, NULL);
		do {
			fwrite(buf , buf_size, 1, fp);
			cur_size += buf_size;
		} while( cur_size < total);
		fsync(fileno(fp));
		gettimeofday(&end, NULL);
	}

	time_in_s = (double)((end.tv_sec - start.tv_sec) +
		((end.tv_usec - start.tv_usec) / 1000000.0));
	Speed_in_MB = (double)(cur_size/MB(1)/time_in_s);
	printf("Total write size:     %zu Bytes\n", cur_size);
	printf("Write size each time: %zu Bytes\n", buf_size);
	printf("Write time:            %f Seconds\n", (double)time_in_s);
	printf("Write speed:           %f MB/s\n", (double)Speed_in_MB);

	return Speed_in_MB;
}

static double MeasureFIX_ReadSpeed(char *buf, size_t buf_size, size_t total)
{
	struct timeval start, end;
	double Speed_in_MB = 0, time_in_s = 0;
	size_t cur_size = 0;

	memset(buf, 0x1F, buf_size);

	if (file_op == SYSTEM_CALL) {
		lseek(fd, 0x0, SEEK_SET);
	} else if (file_op == FILE_STREAM) {
		fseek(fp, 0x0, SEEK_SET);
	}
	drop_cache();

	if (file_op == SYSTEM_CALL) {
		gettimeofday(&start, NULL);
		do {
			cur_size += read(fd, buf, buf_size);
		} while( cur_size < total);
		gettimeofday(&end, NULL);
	} else if (file_op == FILE_STREAM) {
		gettimeofday(&start, NULL);
		do {
			fread(buf , buf_size, 1, fp);
			cur_size += buf_size;
		} while( cur_size < total);
		gettimeofday(&end, NULL);
	}

	time_in_s = (double)((end.tv_sec - start.tv_sec) +
		((end.tv_usec - start.tv_usec) / 1000000.0));
	Speed_in_MB = (double)(cur_size/MB(1)/time_in_s);
	printf("Total read  size:     %zu Bytes\n", cur_size);
	printf("Read  size each time: %zu Bytes\n", buf_size);
	printf("Read  time:            %f Seconds\n", (double)time_in_s);
	printf("Read  speed:           %f MB/s\n", (double)Speed_in_MB);

	return Speed_in_MB;
}

static double MeasureRAND_WriteSpeed(size_t total)
{
	struct timeval start, end;
	double Speed_in_MB = 0, time_in_s = 0;
	size_t cur_size = 0;
	int i;

	for (i = 0; i < RAND_NUM_DATA_SIZE; i++)
		memset(_gstRandBuf[i].buf, 0x1F, _gstRandBuf[i].buf_size);

	if (file_op == SYSTEM_CALL) {
		lseek(fd, 0x0, SEEK_SET);
	} else if (file_op == FILE_STREAM) {
		fseek(fp, 0x0, SEEK_SET);
	}
	drop_cache();

	if (file_op == SYSTEM_CALL) {
		gettimeofday(&start, NULL);
		for (i = 0; i < RAND_NUM_DATA_SIZE && cur_size < total; i++) {
			cur_size += write(fd, _gstRandBuf[i].buf, _gstRandBuf[i].buf_size);
			if (i == RAND_NUM_DATA_SIZE - 1)
				i = 0;
		}
		gettimeofday(&end, NULL);
	} else if (file_op == FILE_STREAM) {
		gettimeofday(&start, NULL);
		for (i = 0; i < RAND_NUM_DATA_SIZE && cur_size < total; i++) {
			fwrite(_gstRandBuf[i].buf, _gstRandBuf[i].buf_size, 1, fp);
			cur_size += _gstRandBuf[i].buf_size;
			if (i == RAND_NUM_DATA_SIZE - 1)
				i = 0;
		}
		fsync(fileno(fp));
		gettimeofday(&end, NULL);
	}

	time_in_s = (double)((end.tv_sec - start.tv_sec) +
		((end.tv_usec - start.tv_usec) / 1000000.0));
	Speed_in_MB = (double)(cur_size/MB(1)/time_in_s);
	printf("Total write size:     %zu Bytes\n", cur_size);
	printf("Write time:            %f Seconds\n", (double)time_in_s);
	printf("Write speed:           %f MB/s\n", (double)Speed_in_MB);

	return Speed_in_MB;
}

static double MeasureRAND_ReadSpeed(size_t total)
{
	struct timeval start, end;
	double Speed_in_MB = 0, time_in_s = 0;
	size_t cur_size = 0;
	int i;

	for (i = 0; i < RAND_NUM_DATA_SIZE; i++)
		memset(_gstRandBuf[i].buf, 0x1F, _gstRandBuf[i].buf_size);

	if (file_op == SYSTEM_CALL) {
		lseek(fd, 0x0, SEEK_SET);
	} else if (file_op == FILE_STREAM) {
		fseek(fp, 0x0, SEEK_SET);
	}
	drop_cache();

	if (file_op == SYSTEM_CALL) {
		gettimeofday(&start, NULL);
		for (i = 0; i < RAND_NUM_DATA_SIZE && cur_size < total; i++) {
			cur_size += read(fd, _gstRandBuf[i].buf, _gstRandBuf[i].buf_size);
			if (i == RAND_NUM_DATA_SIZE - 1)
				i = 0;
		}
		gettimeofday(&end, NULL);
	} else if (file_op == FILE_STREAM) {
		gettimeofday(&start, NULL);
		for (i = 0; i < RAND_NUM_DATA_SIZE && cur_size < total; i++) {
			fread(_gstRandBuf[i].buf, _gstRandBuf[i].buf_size, 1, fp);
			cur_size += _gstRandBuf[i].buf_size;
			if (i == RAND_NUM_DATA_SIZE - 1)
				i = 0;
		}
		gettimeofday(&end, NULL);
	}

	time_in_s = (double)((end.tv_sec - start.tv_sec) +
		((end.tv_usec - start.tv_usec) / 1000000.0));
	Speed_in_MB = (double)(cur_size/MB(1)/time_in_s);
	printf("Total read  size:     %zu Bytes\n", cur_size);
	printf("Read  time:            %f Seconds\n", (double)time_in_s);
	printf("Read  speed:           %f MB/s\n", (double)Speed_in_MB);

	return Speed_in_MB;
}

static size_t _parseTotalSize(char *input)
{
	char ImageSize[128];

	if (strcspn(input, "K") < strlen(input)) {
		strncpy(ImageSize, input, strcspn(input, "K"));
		return KB(atol(ImageSize));
	} else if (strcspn(input, "M") < strlen(input)) {
		strncpy(ImageSize, input, strcspn(input, "M"));
		return MB(atol(ImageSize));
	} else if (strcspn(input, "G") < strlen(input)) {
		strncpy(ImageSize, input, strcspn(input, "G"));
		return GB(atol(ImageSize));
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 6) {
		strncpy(mode, argv[1], sizeof(mode));
		strncpy(op_mode, argv[2], sizeof(op_mode));
		repeat_times = atoi(argv[3]);
		strncpy(file_path, argv[4], sizeof(file_path));
		stImageSize = _parseTotalSize(argv[5]);
		if (repeat_times <= 0) {
			printf("Repeat:%d is invalid, set default value\n", repeat_times);
			repeat_times = 1;
		}
		if (stImageSize < FIX_DATA_SIZE) {
			printf("Please check TOTAL_SIZE:%zu, should be larger than 512KB\n", stImageSize);
			goto EXIT;
		}
		if ((0 != strcmp(op_mode, "SYSCALL")) && (0 != strcmp(op_mode, "STREAM"))) {
			printf("Please check OP argument: %s, should be either SYSCALL or STREAM\n", op_mode);
			goto EXIT;
		} else {
			if (!strcmp(op_mode, "SYSCALL"))
				file_op = SYSTEM_CALL;
			else if(!strcmp(op_mode, "STREAM"))
				file_op = FILE_STREAM;
		}
	} else {
		if ((argc == 2) &&
			((0 == strcmp(argv[1], "--help")) || (0 == strcmp(argv[1], "-h")))) {
			printf("==================Usage==================\n");
			printf("profileSD [MODE] [OP] [LOOP] [PATH] [SIZE]\n");
			printf("[MODE]: FIX\n");
			printf("write/read data in the fixed length of 512 KBytes in a single channel.\n");
			printf("-\n");
			printf("[MODE]: RAND\n");
			printf("randomly and cyclically write/read data in the following different lengths"
				"(in bytes) in a single channel: 29, 30290, 3435, 235, 12345, 80, 9845, 564,"
				"34862, 123, 267890, 36, 6788, 86, 234567, 1232, 514, 50, 678, 9864, 333333.\n");
			printf("-\n");
			printf("[OP]: SYSCALL/STREAM\n");
			printf("-\n");
			printf("[LOOP]: Repeat Times: default 1\n");
			printf("-\n");
			printf("[PATH]: Generated file path\n");
			printf("-\n");
			printf("[LOOP]: Generated file size\n");
			printf("==================Example==================\n");
			printf("./BinaryName MODE       OP   LOOP  PATH                SIZE\n");
			printf("./profileSD   FIX  SYSCALL    5   \"/mnt/sd/test.img\" 1024MB\n");
			printf("./profileSD  RAND   STREAM    5   \"/mnt/sd/test.img\" 1GB\n");
		} else {
			printf("==================Usage==================\n");
			printf("profileSD [MODE] [OP] [LOOP] [PATH] [SIZE]\n");
			printf("Try 'profileSD --help' for more information.\n");
		}
		goto EXIT;
	}
	if (0 == strcmp(mode ,"FIX")) {
		if (file_op == SYSTEM_CALL) {
			fd = open(file_path, O_CREAT | O_RDWR | O_SYNC, 0666);
		} else if (file_op == FILE_STREAM) {
			fp = fopen(file_path, "w+");
		}

		char *Buffer = NULL;
		if (fd > 0 || fp != NULL) {
			double AvrWSpeedMB = 0, AvrRSpeedMB = 0;
			int loop = 1;

			Buffer = malloc(FIX_DATA_SIZE);
			if (Buffer == NULL)
				goto EXIT;

			printf("==================Test Start==================\n");
			printf("Generate File name: %s, mode:%s operation:%s\n", file_path, mode, op_mode);
			printf("(It may cost few seconds, please don't close it.)\n");

			do {
				printf("*************Trying %d/%d rounds*************\n", loop, repeat_times);
				AvrWSpeedMB += MeasureFIX_WriteSpeed(Buffer, FIX_DATA_SIZE, stImageSize);
				AvrRSpeedMB += MeasureFIX_ReadSpeed(Buffer, FIX_DATA_SIZE, stImageSize);
				loop++;
			} while( loop <= repeat_times);

			printf("==================Test Result=================\n");
			printf("Average WRITE Speed:  %f MB/s\n", (double)AvrWSpeedMB/repeat_times);
			printf("Average READ  speed:  %f MB/s\n", (double)AvrRSpeedMB/repeat_times);
			printf("==================Test End====================\n");
		} else {
			fprintf(stderr, "Open %s failed due to %s\n", file_path, strerror(errno));
		}
		if (Buffer)
			free(Buffer);
		if (fd)
			close(fd);
		if (fp)
			fclose(fp);
	} else if (0 == strcmp(mode ,"RAND")) {
		int i = 0;

		if (file_op == SYSTEM_CALL) {
			fd = open(file_path, O_CREAT | O_RDWR | O_SYNC, 0666);
		} else if (file_op == FILE_STREAM) {
			fp = fopen(file_path, "w+");
		}

		if (fd > 0 || fp != NULL) {
			double AvrWSpeedMB = 0, AvrRSpeedMB = 0;
			int loop = 1;

			for (i = 0; i < RAND_NUM_DATA_SIZE; i++) {
				_gstRandBuf[i].buf = malloc(_gstRandBuf[i].buf_size);
				if (_gstRandBuf[i].buf == NULL) {
					printf("Allocate %d buffer failed, size: %zu\n", i, _gstRandBuf[i].buf_size);
					goto EXIT;
				}
			}

			printf("==================Test Start==================\n");
			printf("Generate File name: %s mode:%s operation:%s\n", file_path, mode, op_mode);
			printf("(It may cost few seconds, please don't close it.)\n");

			do {
				printf("*************Trying %d/%d rounds*************\n", loop, repeat_times);
				AvrWSpeedMB += MeasureRAND_WriteSpeed(stImageSize);
				AvrRSpeedMB += MeasureRAND_ReadSpeed(stImageSize);
				loop++;
			} while( loop <= repeat_times);

			printf("==================Test Result=================\n");
			printf("Average WRITE Speed:  %f MB/s\n", (double)AvrWSpeedMB/repeat_times);
			printf("Average READ  speed:  %f MB/s\n", (double)AvrRSpeedMB/repeat_times);
			printf("==================Test End====================\n");
		} else {
			fprintf(stderr, "Open %s failed due to %s\n", file_path, strerror(errno));
		}

		for (i = 0; i < RAND_NUM_DATA_SIZE; i++) {
			if(_gstRandBuf[i].buf) {
				free(_gstRandBuf[i].buf);
				_gstRandBuf[i].buf = NULL;
			}
		}
		if (fd)
			close(fd);
		if (fp)
			fclose(fp);
	}
EXIT:
	return 0;
}
