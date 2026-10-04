ISO       = cdvd.iso
ISO_DIR   = iso
IOPRP_SRC = ioprp

# Versioned IOPRP sets — one per version, preferring the set that includes
# CDVDSTM where available (STM B/C over A where both exist).
# Format: $(ISO_DIR)/V<VER>/IOPRP.IMG  [+ CDVDSTM.IRX]
#
# Source directories: ioprp/v<X>.<YZ>/
# File selection per version:
#   no STM  → IOPRP.IMG only
#   STM A   → IOPRP.IMG + CDVDSTM.IRX (v2.60, v2.71: only A available)
#   STM B   → IOPRP.IMG + CDVDSTM.IRX (v2.42, v2.70, v2.80: B preferred)
#   STM C   → IOPRP.IMG + CDVDSTM.IRX (v3.00, v3.10: C = B with minor optimisation)

define cp_ioprp
	mkdir -p $(ISO_DIR)/$(1)
	cp $(IOPRP_SRC)/$(2)/$(3) $(ISO_DIR)/$(1)/IOPRP.IMG
endef
define cp_stm
	cp $(IOPRP_SRC)/$(1)/$(2) $(ISO_DIR)/$(3)/CDVDSTM.IRX
endef

IOP_MODULES = iop/cdvd iop/memmap

all:
	$(foreach m,$(IOP_MODULES),$(MAKE) -C $(m) all;)
	$(MAKE) -C ee all

iso: all
	mkdir -p $(ISO_DIR)
	printf 'BOOT2 = cdrom0:\\CDVD.ELF;1\nVER = 1.00\nVMODE = PAL\n' > $(ISO_DIR)/SYSTEM.CNF
	cp ee/cdvd.elf $(ISO_DIR)/CDVD.ELF
	cp iop/cdvd/cdvd.irx $(ISO_DIR)/CDVD.IRX
	cp iop/memmap/memmap.irx $(ISO_DIR)/MEMMAP.IRX
	# Versioned IOPRP folders — one per firmware version
	$(call cp_ioprp,V140,v1.40,ioprp_43845.img)
	$(call cp_ioprp,V146,v1.46,ioprp_78421.img)
	$(call cp_ioprp,V150,v1.50,ioprp_82741.img)
	$(call cp_ioprp,V160,v1.60,ioprp_91909.img)
	$(call cp_ioprp,V165,v1.65,ioprp_98901.img)
	$(call cp_ioprp,V202,v2.02,ioprp_110993.img)
	$(call cp_ioprp,V205,v2.05,ioprp_119797.img)
	$(call cp_ioprp,V210,v2.10,ioprp_126857.img)
	$(call cp_ioprp,V211,v2.11,ioprp_129577.img)
	$(call cp_ioprp,V213,v2.13,ioprp_129577.img)
	$(call cp_ioprp,V214,v2.14,ioprp_140945.img)
	$(call cp_ioprp,V220,v2.20,ioprp_199257.img)
	$(call cp_ioprp,V221,v2.21,ioprp_196937.img)
	$(call cp_ioprp,V224,v2.24,ioprp_201065.img)
	$(call cp_ioprp,V230,v2.30,ioprp_230329.img)
	$(call cp_ioprp,V234,v2.34,ioprp_247641.img)
	$(call cp_ioprp,V241,v2.41,ioprp_251049.img)
	$(call cp_ioprp,V242,v2.42,ioprp_252409.img)
	$(call cp_ioprp,V243,v2.43,ioprp_253201.img)
	$(call cp_ioprp,V250,v2.50,ioprp_264897.img)
	$(call cp_ioprp,V252,v2.52,ioprp_265233.img)
	$(call cp_ioprp,V253,v2.53,ioprp_267217.img)
	$(call cp_ioprp,V255,v2.55,ioprp_264449.img)
	$(call cp_ioprp,V260,v2.60,ioprp_248945.img)
	$(call cp_stm,v2.60,cdvdstm_248945_24697.irx,V260)
	$(call cp_ioprp,V270,v2.70,ioprp_249121.img)
	$(call cp_stm,v2.70,cdvdstm_249121_34705.irx,V270)
	$(call cp_ioprp,V271,v2.71,ioprp_266817.img)
	$(call cp_stm,v2.71,cdvdstm_266817_24721.irx,V271)
	$(call cp_ioprp,V280,v2.80,ioprp_269889.img)
	$(call cp_stm,v2.80,cdvdstm_269889_34705.irx,V280)
	$(call cp_ioprp,V300,v3.00,ioprp_278305.img)
	$(call cp_stm,v3.00,cdvdstm_278305_33521.irx,V300)
	$(call cp_ioprp,V310,v3.10,ioprp_278353.img)
	$(call cp_stm,v3.10,cdvdstm_278353_33521.irx,V310)
	# Test data file — 512KB (256+ sectors for read tests)
	dd if=/dev/urandom of=$(ISO_DIR)/TEST.BIN bs=1024 count=512
	# Subdirectory test file — for sceCdSearchFile directory-traversal test
	mkdir -p $(ISO_DIR)/SUBDIR
	dd if=/dev/urandom of=$(ISO_DIR)/SUBDIR/TEST.BIN bs=1024 count=4
	mkisofs -o $(ISO) $(ISO_DIR)

install: iso
	cp -f $(ISO) /mnt/truenas_data_exthdd/spellen/PS2/CD/

sim: iso
	flatpak --filesystem=host run net.pcsx2.PCSX2 $(PWD)/$(ISO)

IOPRP_VERS = V140 V146 V150 V160 V165 V202 V205 V210 V211 V213 V214 \
             V220 V221 V224 V230 V234 V241 V242 V243 V250 V252 V253 \
             V255 V260 V270 V271 V280 V300 V310

clean:
	$(foreach m,$(IOP_MODULES),$(MAKE) -C $(m) clean;)
	$(MAKE) -C ee clean
	rm -f $(ISO_DIR)/SYSTEM.CNF $(ISO_DIR)/CDVD.ELF $(ISO_DIR)/CDVD.IRX $(ISO_DIR)/MEMMAP.IRX $(ISO_DIR)/TEST.BIN
	rm -rf $(addprefix $(ISO_DIR)/,$(IOPRP_VERS)) $(ISO_DIR)/SUBDIR
	rm -f $(ISO)
