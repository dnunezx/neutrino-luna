#ifndef MCEMU_CONFIG_H
#define MCEMU_CONFIG_H

#include <stdint.h>
#include "module_config.h"

#define MCEMU_PORTS 2

typedef struct {
    uint16_t PageSize;
    uint16_t BlockSize;
    uint32_t CardSize; /* Number of pages */
} McSpec;

typedef struct {
    int32_t active;
    int32_t flags;
    McSpec cspec;
} McImageSpec;

struct mcemu_settings {
    uint32_t magic;
    McImageSpec card[MCEMU_PORTS];
};

#endif
