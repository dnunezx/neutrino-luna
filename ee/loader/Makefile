GIT_TAG = $(shell git describe --tags)

EE_OBJS = main.o compat.o xparam.o patch.o ee_core_config.o ioprp.o toml.o iso_cnf.o
EE_INCS = -I../ee_core/include
EE_LIBS = -lfileXio -lpatches
EE_CFLAGS = -DGIT_TAG=\"$(GIT_TAG)\"

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

# Optional developer targets and game selections stay local.
-include local/development.mk

clean:
	rm -rf $(EE_OBJS) *_irx.o *_elf.o
	rm -rf $(EE_BIN) $(EE_BIN_PACKED)
	rm -rf modules

copy:
	rm -rf modules
	mkdir -p modules
	cp ../../iop/atad_emu/irx/atad_emu.irx         modules
	cp ../../iop/bdfs/irx/bdfs.irx                 modules
	cp ../../iop/cdvdfsv/irx/cdvdfsv.irx           modules
	cp ../../iop/cdvdman_emu/irx/cdvdman_emu.irx   modules
	cp ../../iop/cdvdman_esr1/irx/cdvdman_esr1.irx modules
	cp ../../iop/cdvdman_esr2/irx/cdvdman_esr2.irx modules
	cp ../../iop/dev9/irx/dev9_hidden.irx          modules
	cp ../../iop/dev9/irx/dev9_ns.irx              modules
	cp ../../iop/fakemod/irx/fakemod.irx           modules
	cp ../../iop/fhi_bd/irx/fhi_bd.irx             modules
	cp ../../iop/fhi_bd_defrag/irx/fhi_bd_defrag.irx modules
	cp ../../iop/gapfill/irx/gapfill.irx           modules
	cp ../../iop/hdlfs/irx/hdlfs.irx               modules
	cp ../../iop/imgdrv/irx/imgdrv.irx             modules
	cp ../../iop/mc_emu/irx/mc_emu.irx             modules
	cp ../../iop/patch_freemem/irx/patch_freemem.irx modules
	cp ../../iop/patch_membo/irx/patch_membo.irx   modules
	cp ../../iop/patch_rc_uya/irx/patch_rc_uya.irx modules
	cp ../../iop/smap_udpbd/irx/smap_udpbd.irx     modules
	cp ../../iop/smap_udptty/irx/smap_udptty.irx   modules
	cp ../../iop/usbd_null/irx/usbd_null.irx       modules
	cp ../ee_core/ee_core.elf                      modules
	cp $(PS2SDK)/iop/irx/udnl.irx                  modules
	cp $(PS2SDK)/iop/irx/udnl-t300.irx             modules
	cp $(PS2SDK)/iop/irx/eesync.irx                modules
	cp $(PS2SDK)/iop/irx/iomanX.irx                modules
	cp $(PS2SDK)/iop/irx/fileXio.irx               modules
	cp $(PS2SDK)/iop/irx/bdm.irx                   modules
	cp $(PS2SDK)/iop/irx/bdmfs_fatfs.irx           modules
	cp $(PS2SDK)/iop/irx/ata_bd.irx                modules
	cp $(PS2SDK)/iop/irx/ps2hdd-bdm.irx            modules
	cp $(PS2SDK)/iop/irx/usbd_mini.irx             modules
	cp $(PS2SDK)/iop/irx/usbmass_bd_mini.irx       modules
	cp $(PS2SDK)/iop/irx/mx4sio_bd_mini.irx        modules
	cp $(PS2SDK)/iop/irx/iLinkman.irx              modules
	cp $(PS2SDK)/iop/irx/IEEE1394_bd_mini.irx      modules

# Add later when modules are part of ps2sdk
#	cp $(PS2SDK)/iop/irx/mmceman.irx               modules
#	cp $(PS2SDK)/iop/irx/mmcefhi.irx               modules

include $(PS2SDK)/Defs.make
include ../Rules.make
