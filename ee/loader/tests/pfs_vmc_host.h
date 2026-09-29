#ifndef PFS_VMC_HOST_H
#define PFS_VMC_HOST_H

#include <stdint.h>

#define FHI_MAX_FILES 6
#define FHI_FID_MC0 4
#define FHI_FID_MC1 5
#define BDM_MAX_FRAGS 64
#define APA_TYPE_PFS 0x0100
#define PDIOC_ZONESZ 0x6807
#define HDIOC_READSECTOR 0x4801
#define FIO_MT_RDONLY 1

typedef struct { uint32_t lba, size; } hddAtaTransfer_t;
typedef struct { uint32_t mode, size, private_4, private_5; } iox_stat_t;
typedef struct { uint64_t sector; uint32_t count; } bd_fragment_t;
struct fhi_bd_file { uint8_t frag_start, frag_count; uint64_t size; };
struct fhi_bd {
    struct fhi_bd_file file[FHI_MAX_FILES];
    bd_fragment_t frags[BDM_MAX_FRAGS];
};

int fileXioDevctl(const char *path, int command, void *args, int args_len,
                  void *output, int output_len);
int fileXioGetStat(const char *path, iox_stat_t *stat);

#endif
