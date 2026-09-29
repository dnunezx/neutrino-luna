#define _POSIX_C_SOURCE 200809L
#define PFS_VMC_HOST_TEST
#include "../src/pfs_vmc.c"

#include <assert.h>
#include <stdlib.h>

#define CARD_SECTORS (8U * 1024U * 1024U / 512U)
#define PART_START 1024U
#define PART_LENGTH 20000U
#define INODE_SECTOR 128U
#define DATA_ZONE 50U
#define DATA_SECTOR (PART_START + DATA_ZONE * PFS_ZONE_SECTORS)

static int raw_fd;
static int zone_size = 8192;
static const char *card_path = "pfs0:card.bin";

int fileXioDevctl(const char *path, int command, void *args, int args_len,
                  void *output, int output_len)
{
    if (!strcmp(path, "pfs0:") && command == PDIOC_ZONESZ)
        return zone_size;
    if (strcmp(path, "hdd0:") || command != HDIOC_READSECTOR ||
        args_len != sizeof(hddAtaTransfer_t))
        return -1;
    hddAtaTransfer_t *request = args;
    if (output_len < 0 || (unsigned)output_len != request->size * 512)
        return -1;
    return pread(raw_fd, output, output_len, (off_t)request->lba * 512) == output_len ? 0 : -1;
}

int fileXioGetStat(const char *path, iox_stat_t *stat)
{
    memset(stat, 0, sizeof(*stat));
    if (!strcmp(path, "hdd0:+OPL")) {
        stat->mode = APA_TYPE_PFS;
        stat->private_5 = PART_START;
        return 0;
    }
    if (!strcmp(path, card_path)) {
        stat->size = CARD_SECTORS * 512;
        stat->private_5 = INODE_SECTOR;
        return 0;
    }
    return -1;
}

static void write_at(int fd, off_t offset, const void *data, size_t size)
{
    assert(pwrite(fd, data, size, offset) == (ssize_t)size);
}

static void make_inode(PfsInode *inode)
{
    memset(inode, 0, sizeof(*inode));
    inode->magic = PFS_INODE_MAGIC;
    inode->size = CARD_SECTORS * 512;
    inode->number_data = 2;
    inode->data[1].number = DATA_ZONE;
    inode->data[1].count = CARD_SECTORS / PFS_ZONE_SECTORS;
    for (int i = 1; i < 256; i++)
        inode->checksum += ((uint32_t *)inode)[i];
}

int main(void)
{
    char raw_path[] = "/tmp/luna-pfs-raw-XXXXXX";
    ApaHeader apa = {0};
    PfsInode inode;
    struct fhi_bd settings = {0};
    uint8_t sector[512] = {0};
    int card_fd;

    raw_fd = mkstemp(raw_path);
    assert(raw_fd >= 0);
    assert(ftruncate(raw_fd, 25000LL * 512) == 0);
    unlink(raw_path);
    card_fd = open(card_path, O_RDWR | O_CREAT | O_EXCL, 0600);
    assert(card_fd >= 0);
    assert(ftruncate(card_fd, CARD_SECTORS * 512) == 0);
    sector[0] = 0x73;
    write_at(card_fd, 0, sector, sizeof(sector));
    sector[0] = 0x42;
    write_at(card_fd, (CARD_SECTORS - 1LL) * 512, sector, sizeof(sector));
    write_at(raw_fd, DATA_SECTOR * 512LL, (uint8_t[512]){0x73}, 512);
    write_at(raw_fd, (DATA_SECTOR + CARD_SECTORS - 1LL) * 512,
             sector, sizeof(sector));
    apa.magic = APA_MAGIC;
    apa.start = PART_START;
    apa.length = PART_LENGTH;
    apa.type = APA_TYPE_PFS;
    write_at(raw_fd, PART_START * 512LL, &apa, sizeof(apa));
    make_inode(&inode);
    write_at(raw_fd, (PART_START + INODE_SECTOR) * 512LL, &inode, sizeof(inode));

    assert(pfs_vmc_map_file(card_path, "hdd0:+OPL", &settings, FHI_FID_MC0) == 0);
    assert(settings.file[FHI_FID_MC0].frag_count == 1);
    assert(settings.file[FHI_FID_MC0].size == CARD_SECTORS * 512);
    assert(settings.frags[0].sector == DATA_SECTOR);
    assert(settings.frags[0].count == CARD_SECTORS);

    zone_size = 4096;
    assert(pfs_vmc_map_file(card_path, "hdd0:+OPL", &settings, FHI_FID_MC1) < 0);
    zone_size = 8192;
    inode.checksum++;
    write_at(raw_fd, (PART_START + INODE_SECTOR) * 512LL, &inode, sizeof(inode));
    assert(pfs_vmc_map_file(card_path, "hdd0:+OPL", &settings, FHI_FID_MC1) < 0);
    make_inode(&inode);
    inode.data[1].number = PART_LENGTH / PFS_ZONE_SECTORS;
    inode.checksum = 0;
    for (int i = 1; i < 256; i++)
        inode.checksum += ((uint32_t *)&inode)[i];
    write_at(raw_fd, (PART_START + INODE_SECTOR) * 512LL, &inode, sizeof(inode));
    assert(pfs_vmc_map_file(card_path, "hdd0:+OPL", &settings, FHI_FID_MC1) < 0);
    make_inode(&inode);
    write_at(raw_fd, (PART_START + INODE_SECTOR) * 512LL, &inode, sizeof(inode));
    sector[0] = 0x00;
    write_at(raw_fd, DATA_SECTOR * 512LL, sector, sizeof(sector));
    assert(pfs_vmc_map_file(card_path, "hdd0:+OPL", &settings, FHI_FID_MC1) < 0);

    close(card_fd);
    unlink(card_path);
    close(raw_fd);
    puts("APA/PFS VMC mapping: ok");
    return 0;
}
