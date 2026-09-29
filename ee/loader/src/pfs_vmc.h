#ifndef PFS_VMC_H
#define PFS_VMC_H

#ifdef PFS_VMC_HOST_TEST
#include "../tests/pfs_vmc_host.h"
#else
#include "../../../common/include/fhi_bd_config.h"
#endif

// Map an existing, fixed-size PFS VMC into the block-device FHI table.
// The PFS partition must already be mounted at pfs0: in the load environment.
// No mapping is committed unless every extent and sampled sector validates.
int pfs_vmc_map_file(const char *path, const char *partition,
                     struct fhi_bd *settings, int file_id);

#endif
