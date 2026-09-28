#ifndef VMC_IMAGE_H
#define VMC_IMAGE_H

#include "../../../common/include/mcemu_config.h"

/* Accepts raw OPL-style PS2 card images and returns the card's geometry. */
int vmc_image_inspect(const char *path, McImageSpec *spec);

#endif
