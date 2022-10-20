#ifndef __HELLO_H__
#define __HELLO_H__

struct buf_info {
    int fd;
    void *buf;
    int size;
};

#define HELLO_MAGIC  't'
#define TEST_DRIVERA    (_IOWR(HELLO_MAGIC, 0x1, struct buf_info))
#define TEST_DRIVERB    (_IOWR(HELLO_MAGIC, 0x2, struct buf_info))

#endif