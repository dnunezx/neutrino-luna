#include "vmc_image.h"

#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define VMC_PAGE_SIZE 512U
#define VMC_MIN_SIZE (8U * 1024U * 1024U)
#define VMC_MAX_SIZE (64U * 1024U * 1024U)

static uint16_t read_u16le(const unsigned char *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t read_u32le(const unsigned char *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

int vmc_image_inspect(const char *path, McImageSpec *spec)
{
    unsigned char superblock[52];
    uint16_t page_size, pages_per_cluster, block_size;
    uint32_t clusters, pages;
    uint64_t expected_size;
    off_t actual_size;
    int fd;

    if (path == NULL || spec == NULL)
        return -1;
    fd = open(path, O_RDONLY);
    if (fd < 0)
        return -1;
    if (read(fd, superblock, sizeof(superblock)) != sizeof(superblock)) {
        close(fd);
        return -1;
    }
    actual_size = lseek(fd, 0, SEEK_END);
    close(fd);
    if (actual_size < 0 ||
        memcmp(superblock, "Sony PS2 Memory Card Format ", 28) != 0)
        return -1;

    page_size = read_u16le(superblock + 40);
    pages_per_cluster = read_u16le(superblock + 42);
    block_size = read_u16le(superblock + 44);
    clusters = read_u32le(superblock + 48);
    expected_size = (uint64_t)page_size * pages_per_cluster * clusters;
    if (page_size != VMC_PAGE_SIZE || pages_per_cluster != 2 ||
        block_size < 16 || block_size > 64 ||
        (block_size & (block_size - 1)) != 0 ||
        expected_size < VMC_MIN_SIZE || expected_size > VMC_MAX_SIZE ||
        (expected_size & (expected_size - 1)) != 0 ||
        expected_size != (uint64_t)actual_size)
        return -1;

    pages = pages_per_cluster * clusters;
    if (pages % block_size != 0)
        return -1;
    memset(spec, 0, sizeof(*spec));
    spec->active = 1;
    spec->flags = 0x2b | 0x100;
    spec->cspec.PageSize = page_size;
    spec->cspec.BlockSize = block_size;
    spec->cspec.CardSize = pages;
    return 0;
}
