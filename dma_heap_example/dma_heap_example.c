#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>

#define DMA_HEAP_DEV "/dev/dma_heap/system"
#define DMA_HEAP_IOC_MAGIC 'H'

struct dma_heap_allocation_data {
    uint64_t len;
    uint32_t fd;
    uint32_t fd_flags;
    uint64_t heap_flags;
};

#define DMA_HEAP_IOCTL_ALLOC _IOWR(DMA_HEAP_IOC_MAGIC, 0, struct dma_heap_allocation_data)

int main(int argc, char *argv[])
{
    int heap_fd, buf_fd;
    void *buf_ptr;
    struct dma_heap_allocation_data alloc_data;
    FILE *fp;
    size_t file_size;
    struct stat st;
    int i;
    int j;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <buffer_size>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    size_t buffer_size = strtoull(argv[2], NULL, 10);

    printf("=== DMA Heap Memory Allocation Example ===\n");
    printf("Input file: %s\n", input_file);
    printf("Buffer size: %zu bytes\n", buffer_size);

    /* Step 1: 检查文件是否存在 */
    if (stat(input_file, &st) != 0) {
        fprintf(stderr, "Error: stat failed for %s: %s (errno=%d)\n",
                input_file, strerror(errno), errno);
        return 1;
    }
    printf("1. File exists: size=%zu bytes, mode=%o\n", st.st_size, st.st_mode);

    /* Step 2: Open DMA Heap device */
    heap_fd = open(DMA_HEAP_DEV, O_RDWR);
    if (heap_fd < 0) {
        fprintf(stderr, "Failed to open %s: %s (errno=%d)\n",
                DMA_HEAP_DEV, strerror(errno), errno);
        return 1;
    }
    printf("2. Opened %s successfully (fd=%d)\n", DMA_HEAP_DEV, heap_fd);

    /* Step 3: Allocate memory from DMA Heap */
    memset(&alloc_data, 0, sizeof(alloc_data));
    alloc_data.len = buffer_size;
    alloc_data.fd_flags = O_RDWR | O_CLOEXEC;

    if (ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &alloc_data) < 0) {
        fprintf(stderr, "Failed to allocate DMA buffer: %s (errno=%d)\n",
                strerror(errno), errno);
        close(heap_fd);
        return 1;
    }
    buf_fd = alloc_data.fd;
    printf("3. Allocated DMA buffer successfully (fd=%d)\n", buf_fd);

    /* Step 4: Map DMA buffer to user space with MAP_SHARED_VALIDATE */
    printf("4. Mapping DMA buffer to user space...\n");
    buf_ptr = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);
    if (buf_ptr == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap DMA buffer: %s (errno=%d)\n",
                strerror(errno), errno);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }
    printf("   -> Mapped to address: %p\n", buf_ptr);
    memset(buf_ptr, 0, buffer_size);
    /* Step 5: 同步内存区域（关键修复） */
    /*
    printf("5. Syncing memory region...\n");
    if (msync(buf_ptr, buffer_size, MS_SYNC) != 0) {
        fprintf(stderr, "Warning: msync failed: %s (errno=%d)\n", strerror(errno), errno);
    } else {
        printf("   -> Memory sync completed successfully\n");
    }
    */

    /* Step 6: 验证内存可写性 */
    printf("6. Testing memory write access...\n");
    char *test_ptr = (char *)buf_ptr;
    for (i = 0; i < buffer_size; i++) {
        test_ptr[i] = i & 0xff;
    }

    for (j = 0; j < buffer_size; j++) {
        if (test_ptr[j] != (j & 0xff)) {
            break;
        }
    }

    if (j == buffer_size) {
        printf("   -> Memory write test passed\n");
    } else {
        fprintf(stderr, "   -> ERROR: Memory write test failed!\n");
        munmap(buf_ptr, buffer_size);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }

    /* Step 7: Open input file */
    fp = fopen(input_file, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open input file %s: %s (errno=%d)\n",
                input_file, strerror(errno), errno);
        munmap(buf_ptr, buffer_size);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }
    printf("7. Opened input file: %s\n", input_file);

    /* Step 8: Get file size */
    if (fstat(fileno(fp), &st) != 0) {
        fprintf(stderr, "Error: fstat failed: %s (errno=%d)\n", strerror(errno), errno);
        fclose(fp);
        munmap(buf_ptr, buffer_size);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }
    file_size = st.st_size;
    printf("8. File size: %zu bytes\n", file_size);

    /* Step 9: 确保文件指针在开头 */
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Error: fseek failed: %s (errno=%d)\n", strerror(errno), errno);
        fclose(fp);
        munmap(buf_ptr, buffer_size);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }
    printf("9. File pointer positioned at beginning\n");

    /* Step 10: 使用 fread() 读取文件内容 */
    printf("10. Reading file content using fread()...\n");
    errno = 0;
    //file_size = 8*1024;
    size_t bytes_read = fread(buf_ptr, 1, file_size, fp);
    printf("    -> fread() returned: %zu bytes, errno=%d (%s)\n",
           bytes_read, errno, strerror(errno));

    if (bytes_read == 0 && !feof(fp)) {
        fprintf(stderr, "Error: fread() failed: %s (errno=%d)\n", strerror(errno), errno);
        fclose(fp);
        munmap(buf_ptr, buffer_size);
        close(buf_fd);
        close(heap_fd);
        return 1;
    }

    /* Step 11: 再次同步内存 */
    /*
    printf("11. Syncing memory after write...\n");
    if (msync(buf_ptr, bytes_read, MS_SYNC) != 0) {
        fprintf(stderr, "Warning: msync after write failed: %s (errno=%d)\n", strerror(errno), errno);
    }
    */

    /* Step 12: Verify data */
    printf("12. Verifying data...\n");
    if (bytes_read > 0) {
        printf("    -> First 32 bytes of buffer content:\n");
        unsigned char *data = (unsigned char *)buf_ptr;
        for (size_t i = 0; i < (bytes_read < 32 ? bytes_read : 32); i++) {
            printf("%02x ", data[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    } else {
        printf("    -> No data read\n");
    }

    /* Step 13: Cleanup */
    fclose(fp);
    printf("13. Closed input file\n");

    munmap(buf_ptr, buffer_size);
    printf("14. Unmapped DMA buffer\n");

    close(buf_fd);
    printf("15. Closed DMA buffer fd\n");

    close(heap_fd);
    printf("16. Closed DMA heap fd\n");

    if (bytes_read == 0) {
        fprintf(stderr, "=== ERROR: Failed to read data from file ===\n");
        return 1;
    }

    printf("=== All operations completed successfully ===\n");
    return 0;
}