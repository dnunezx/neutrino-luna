# LUNA 2026
GIT_TAG = $(shell git describe --tags --exclude=latest)

EE_OBJS = main.o modlist.o config.o fhi_config.o pfs_vmc.o vmc_image.o xparam.o patch.o ioprp.o tomlc17.o iso_cnf.o mmce.o mmceman_irx.o sio2man_irx.o
EE_LIBS = -lfileXio -lpatches
EE_CFLAGS = -DGIT_TAG=\"$(GIT_TAG)\"

ifeq ($(LUNA_EMULATOR_HOST_IGR),1)
EE_CFLAGS += -DLUNA_EMULATOR_HOST_IGR
endif

EE_NEWLIB_NANO = 1
EE_COMPACT_EXECUTABLE = 1

EE_BIN_NAME = neutrino
EE_BIN = $(EE_BIN_NAME)_unpacked.elf
EE_BIN_PACKED = $(EE_BIN_NAME).elf

$(EE_BIN_PACKED): $(EE_BIN)
	ps2-packer $< $@ > /dev/null

version.txt: FORCE
	echo $(GIT_TAG) > $@
FORCE:

all: $(EE_BIN_PACKED) version.txt

# Optional developer targets and game selections are maintained locally.
-include local/development.mk

clean:
	rm -rf $(EE_OBJS) *_irx.o *_elf.o
	rm -rf $(EE_BIN) $(EE_BIN_PACKED)
	rm -rf modules

include $(PS2SDK)/Defs.make
include ../Rules.make

$(EE_OBJS_DIR)%_irx.c: $(PS2SDK)/iop/irx/%.irx
	@mkdir -p $(EE_OBJS_DIR)
	$(PS2SDK)/bin/bin2c $< $@ $*_irx

$(EE_OBJS_DIR)%_irx.o: $(EE_OBJS_DIR)%_irx.c
	$(EE_CC) $(EE_CFLAGS) -c $< -o $@
