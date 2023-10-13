#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ion_cvitek.h"

int main(int argc, char *argv[])
{
	int ret, ionfd;

    struct ion_custom_data custom;
	struct cvitek_heap_info head_info;

	ionfd = open("/dev/ion", O_RDWR);
	if (ionfd < 0) {
		fprintf(stderr, "<%s>: Failed to open ion client:\n",
			__func__);
		return -1;
	}

	memset(&custom, 0, sizeof(custom));
	custom.cmd = ION_IOC_CVITEK_GET_HEAP_INFO;
	custom.arg = &head_info;
	head_info.id = 0;
	ret = ioctl(ionfd, ION_IOC_CUSTOM, &custom);
	if (ret < 0) {
		fprintf(stderr, "<%s>: Failed: ION_IOC_CUSTOM\n",
			__func__);
		goto exit;
	}
	fprintf(stderr, "head_id=%d, total_size=0x%lx, avail_size=0x%lx\n",
		head_info.id, head_info.total_size, head_info.avail_size);

exit:
	if (ionfd)
		close(ionfd);

	return 0;
}
