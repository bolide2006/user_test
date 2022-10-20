#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <linux/dma-heap.h>
#include "hello.h"

int main(void)
{
	int heap_fd = -1;
	int ret = -1;
	int buf_fd = -1;
	int fda = -1;
	int fdb = -1;
	void *p = NULL;
	struct dma_heap_allocation_data data;
	struct buf_info infoa = {0};
	struct buf_info infob = {0};

	/* open dma-heap */
	heap_fd = open("/dev/dma_heap/linux,cma", O_RDWR);
	if (heap_fd < 0)
	{
		printf("Error! failed to open /dev/dma_heap/linux,cma");
		return -1;
	}

	data.len = 1024 * 1024; // 1M
	data.fd = 0;
	data.fd_flags = O_RDWR | O_CLOEXEC;
	data.heap_flags = 0;

	/* alloc buf */
	ret = ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &data);
	buf_fd = (int)data.fd;
	
	p = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);
	printf("buf_fd=%d p=%p", buf_fd, p);
	/* driver A */
	fda = open("/dev/cdriverA", O_RDWR);

	infoa.fd = buf_fd;
	infoa.buf = p;
	infoa.size = 1024 * 1024; // 1M
	strcpy((char *)infoa.buf, "driverA: userspace!");

	ret = ioctl(fda, TEST_DRIVERA, &infoa);
	printf("driverA buf = %s\n", infoa.buf);

	/* driver B */
	fdb = open("/dev/cdriverB", O_RDWR);

	infob.fd = buf_fd;
	infob.buf = p;
	infob.size = 1024 * 1024; // 1M
	ret = ioctl(fda, TEST_DRIVERB, &infob);
	printf("driverB buf = %s\n", infob.buf);

	return 0;
}
