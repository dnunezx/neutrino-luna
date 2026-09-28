#define _POSIX_C_SOURCE 200809L
#include "../src/vmc_image.h"

#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void write_u16le(unsigned char *p, uint16_t value)
{
    p[0] = value & 0xff;
    p[1] = value >> 8;
}

static void write_u32le(unsigned char *p, uint32_t value)
{
    p[0] = value & 0xff;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

static void make_image(int fd, uint32_t megabytes)
{
    unsigned char header[52] = {0};
    memcpy(header, "Sony PS2 Memory Card Format ", 28);
    memcpy(header + 28, "1.2.0.0", 7);
    write_u16le(header + 40, 512);
    write_u16le(header + 42, 2);
    write_u16le(header + 44, 16);
    write_u32le(header + 48, megabytes * 1024 * 1024 / 1024);
    assert(ftruncate(fd, (off_t)megabytes * 1024 * 1024) == 0);
    assert(lseek(fd, 0, SEEK_SET) == 0);
    assert(write(fd, header, sizeof(header)) == sizeof(header));
}

int main(void)
{
    char path[] = "/tmp/luna-vmc-test-XXXXXX";
    McImageSpec spec;
    int fd = mkstemp(path);
    assert(fd >= 0);

    make_image(fd, 8);
    assert(vmc_image_inspect(path, &spec) == 0);
    assert(spec.active == 1 && spec.cspec.CardSize == 16384);
    assert(spec.cspec.PageSize == 512 && spec.cspec.BlockSize == 16);

    make_image(fd, 16);
    assert(vmc_image_inspect(path, &spec) == 0);
    assert(spec.cspec.CardSize == 32768);

    assert(ftruncate(fd, 16 * 1024 * 1024 + 16) == 0);
    assert(vmc_image_inspect(path, &spec) < 0);

    make_image(fd, 4);
    assert(vmc_image_inspect(path, &spec) < 0);

    make_image(fd, 8);
    assert(lseek(fd, 0, SEEK_SET) == 0);
    assert(write(fd, "bad", 3) == 3);
    assert(vmc_image_inspect(path, &spec) < 0);

    close(fd);
    unlink(path);
    puts("VMC image validation: ok");
    return 0;
}
