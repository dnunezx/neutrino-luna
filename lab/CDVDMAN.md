# CDVDMAN Module Analysis

This document analyzes `cdvdman_*.irx` (IOP module name: `cdvd_driver`) extracted from IOPRP ROM images across PS2 hardware generations. The module is the IOP-side CD/DVD drive manager that directly controls the optical drive hardware. It exports the `cdvdman` library, which is consumed by `cdvdfsv` (the EE-side RPC server) and other IOP modules. The driver manages drive initialization, disc detection, data reading, streaming, media commands, and power control. Across generations it gained new hardware interfaces (DMA via `dmacman`, BIOS memory management via `sysmem`, and extended `ioman` file operations) and progressively expanded its export table to expose more functions.

---

## Summary Table

| File | Version | Text | Data | BSS | Exports (count, highest slot) | Key import changes vs prior |
|------|---------|------|------|-----|-------------------------------|------------------------------|
| 14 | 0x0106 | 14528 | 2320 | 91184 | 62 (0-61) | Baseline: loadcore/intrman/stdio/thbase(2)/thsemap/sysclib/ioman |
| 15 | 0x0207 | 19616 | 2896 | 91484 | 66 (0-65) | thbase gains alarm funcs (35-38) |
| 16 | 0x020a | 21536 | 3040 | 91500 | 69 (0-68) | same as 15 (no thevent yet) |
| 165 | 0x020b | 24752 | 3408 | 91532 | 76 (0-75) | +loadcore[12], +intrman[23](in_irq), +thevent(7 funcs), thsemap+isignal |
| 202 | 0x020d | 25360 | 3344 | 91568 | 76 (0-75) | same libs as 165 |
| 205 | 0x020e | 25888 | 3488 | 91568 | 78 (0-77) | +intrman cpu_suspend/resume(17,18), ioman upgraded 0x0103 |
| 21 | 0x020e | 25952 | 3536 | 91568 | 78 (0-77) | (byte-identical to 210) |
| 210 | 0x020e | 25952 | 3536 | 91568 | 78 (0-77) | same as 205 |
| 211 | 0x0210 | 27712 | 3728 | 92360 | 78 (0-77) | +sysmem(14), thevent+poll(11), thsemap drops poll_sema |
| 213 | 0x0210 | 27712 | 3728 | 92360 | 78 (0-77) | (byte-identical to 211) |
| 214 | 0x0210 | 27712 | 3728 | 92360 | 78 (0-77) | (byte-identical to 211) |
| 224 | 0x0214 | 30368 | 3792 | 91660 | 79 (0-78) | +dmacman(7 funcs), sysclib changes funcs, ioman 0x0104 |
| 23 | 0x0215 | 30720 | 3792 | 91660 | 79 (0-78) | loadcore 0x0102 |
| 234 | 0x0216 | 33552 | 4528 | 51900 | 82 (0-81) | loadcore 0x0103, stdio 0x0103, BSS shrinks significantly |
| 241 | 0x0218 | 35904 | 4544 | 51920 | 82 (0-81) | same libs as 234 |
| 242 | 0x0219 | 35904 | 4544 | 51920 | 82 (0-81) | same libs as 241 |
| 243 | 0x021a | 36096 | 4544 | 51920 | 82 (0-81) | +loadcore[27], export 2 becomes unique (sceCdstm0Cb impl.) |
| 250 | 0x021c | 39520 | 5120 | 52016 | 90 (0-89) | thbase 0x0102; 8 new exports added (slots 82-89) |
| 253 | 0x021d | 40944 | 5184 | 52016 | 113 (0-112) | 23 new exports added (slots 89-112 including sceCdApplySCmd2) |
| 255 | 0x021d | 40944 | 5184 | 52016 | 113 (0-112) | (byte-identical to 253) |
| 260 | 0x0220 | 40224 | 4896 | 35168 | 123 (0-122) | +intrman[7](disable_irq), thevent+refer/irefer(13,14), thsemap reduced to signal only, sysclib+12+14, +ioman[31], BSS shrinks further |
| 270 | 0x0221 | 40320 | 4896 | 35168 | 123 (0-122) | same libs as 260 |
| 271 | 0x0222 | 47328 | 5712 | 35584 | 148 (0-147) | sysmem+alloc/free(4,5), sysclib+19+36, ioman gains many funcs(4-8,10,34) |
| 271_2 | 0x0222 | 47248 | 5680 | 54304 | 148 (0-147) | same lib set as 271 but different BSS/sizes |
| 271a | 0x0222 | 47328 | 5712 | 35584 | 148 (0-147) | (byte-identical to 271) |
| 271b | 0x0222 | 47328 | 5712 | 35584 | 148 (0-147) | (byte-identical to 271) |
| 280 | 0x0223 | 48112 | 5760 | 54304 | 154 (0-153) | sysclib 0x0104 |
| 280a | 0x0223 | 48112 | 5760 | 54304 | 154 (0-153) | (byte-identical to 280) |
| 280b | 0x0223 | 48112 | 5760 | 54304 | 154 (0-153) | (byte-identical to 280) |
| 300 | 0x0225 | 49504 | 5824 | 54304 | 185 (0-184) | sysmem 0x0102; 31 new stub slots added |
| 300_4 | 0x0225 | 49888 | 5840 | 54320 | 185 (0-184) | slightly larger text than 300 |
| 300b | 0x0225 | 49504 | 5824 | 54304 | 185 (0-184) | (byte-identical to 300) |
| 300_2 | 0x0226 | 50464 | 6016 | 54320 | 191 (0-190) | 6 new exports |
| 300_3 | 0x0226 | 50528 | 6016 | 54320 | 191 (0-190) | slightly larger text than 300_2 |
| 300a | 0x0226 | 50464 | 6016 | 54320 | 191 (0-190) | (byte-identical to 300_2) |
| 310 | 0x0226 | 50464 | 6016 | 54320 | 191 (0-190) | (byte-identical to 300_2/300a) |
| 310a | 0x0226 | 50464 | 6016 | 54320 | 191 (0-190) | (byte-identical to 300_2/300a/310) |
| 310b | 0x0226 | 50464 | 6016 | 54320 | 191 (0-190) | (byte-identical to 300_2/300a/310/310a) |

Notes on export count: "62 (0-61)" means exports 0 through 61 (62 total). Export 0 is always `_init_module` at address 0x0000. Exports 1-3 always point to the `_retonly` stub address.

---

## Variant Groups

The following files contain byte-identical or structurally identical binaries (same version, same sizes, same imports, same export table layout):

**Group A — 0x020e/25952/3536:** `cdvdman_21.irx` and `cdvdman_210.irx`
- Same version (0x020e), same sizes (text=25952, data=3536, bss=91568), same export addresses, same import set.

**Group B — 0x0210/27712/3728:** `cdvdman_211.irx`, `cdvdman_213.irx`, `cdvdman_214.irx`
- All three have identical sizes and export tables. These come from three distinct IOPRP ROM versions that shipped the same driver build.

**Group C — 0x021d/40944/5184:** `cdvdman_253.irx` and `cdvdman_255.irx`
- Byte-identical.

**Group D — 0x0222/47328/5712/35584:** `cdvdman_271.irx`, `cdvdman_271a.irx`, `cdvdman_271b.irx`
- All three are byte-identical (text=47328, data=5712, bss=35584).

**Group D2 — 0x0222/47248/5680/54304:** `cdvdman_271_2.irx`
- Same version (0x0222) but different sizes and larger BSS. A different build shipped in a different IOPRP variant.

**Group E — 0x0223/48112/5760/54304:** `cdvdman_280.irx`, `cdvdman_280a.irx`, `cdvdman_280b.irx`
- Byte-identical.

**Group F — 0x0225/49504/5824/54304:** `cdvdman_300.irx`, `cdvdman_300b.irx`
- Byte-identical.

**Group G — 0x0226/50464/6016/54320:** `cdvdman_300_2.irx`, `cdvdman_300a.irx`, `cdvdman_310.irx`, `cdvdman_310a.irx`, `cdvdman_310b.irx`
- All five are byte-identical.

---

## Per-Version Details

For each distinct binary group, the "implemented" exports are those whose address differs from the stub address (the address shared by exports 1, 2, and 3). Export 2 is notable: in early versions it always equals the stub, but from 0x021a onward it becomes a unique address (indicating `sceCdstm0Cb` is now implemented).

---

### cdvdman_14 — Version 0x0106

**Sizes:** text=14528, data=2320, bss=91184

**Imported libraries:**
- `loadcore` 0x0101: func 6
- `intrman` 0x0102: 4 (request_irq), 5 (release_irq), 6 (enable_irq)
- `stdio` 0x0102: 4 (printf)
- `thbase` 0x0101: 20 (get_id), 33 (delay)
- `thsemap` 0x0101: 4 (create), 5 (delete), 6 (signal), 8 (wait), 9 (poll)
- `sysclib` 0x0101: 12, 20, 22, 27, 29, 30
- `ioman` 0x0102: 20, 21

**Exports:** 62 total (slots 0-61). Stub address: 0x3740.

Implemented slots (non-stub, non-zero):
4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 13 (sceCdDiskReady), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 17 (sceCdIntToPos), 21 (sceCdCheckCmd), 28 (sceCdStatus), 29 (sceCdApplySCmd), 37 (sceCdCallback), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 48 (sceCdstm0Cb), 49 (sceCdstm1Cb), 50 (sceCdSC), 51 (sceCdRC), 54 (sceCdApplyNCmd), 56 (sceCdStInit), 57 (sceCdStRead), 58 (sceCdStSeek), 59 (sceCdStStart), 60 (sceCdStStat), 61 (sceCdStStop).

Stub slots: 1, 2, 3, 18, 19, 20, 22 (sceCdRI), 23 ((sceCdWI)), 24 (sceCdReadClock), 25, 26, 27, 30-36, 40-43, 45, 52, 53, 55.

**Notes:** Earliest version. No alarm functions, no thevent, no sysmem, no dmacman. No sceCdRead0/RV/RM/WM/ReadChain/StPause/StResume. Export table ends at slot 61 (sceCdStStop).

---

### cdvdman_15 — Version 0x0207

**Sizes:** text=19616, data=2896, bss=91484

**Imported libraries:**
- `loadcore` 0x0101: 6
- `intrman` 0x0102: 4, 5, 6
- `stdio` 0x0102: 4
- `thbase` 0x0101: 20, 33, 35 (set_alarm), 36 (iset_alarm), 37 (cancel_alarm), 38 (icancel_alarm)
- `thsemap` 0x0101: 4, 5, 6, 8, 9
- `sysclib` 0x0101: 12, 20, 22, 27, 29, 30
- `ioman` 0x0102: 20, 21

**Exports:** 66 total (slots 0-65). Stub address: 0x4b00.

Implemented slots: 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21, 22 (sceCdRI), 23 ((sceCdWI)), 24 (sceCdReadClock), 28, 29, 37, 38, 39, 44, 46, 47, 48, 49, 50, 51, 54, 56, 57, 58, 59, 60, 61, 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 65 ((sceCdWM)).

**Notes:** Alarm functions added. sceCdRI/WI/ReadClock (22-24) become implemented. New slots 62-65 (Read0/RV/RM/WM) added over v14.

---

### cdvdman_16 — Version 0x020a

**Sizes:** text=21536, data=3040, bss=91500

**Imported libraries:** Same as cdvdman_15.

**Exports:** 69 total (slots 0-68). Stub address: 0x527c.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-62, 63, 64, 65, 66 (sceCdReadChain), 67 (sceCdStPause), 68 (sceCdStResume).

**Notes:** Three new slots added over v15: 66 (sceCdReadChain), 67 (sceCdStPause), 68 (sceCdStResume).

---

### cdvdman_165 — Version 0x020b

**Sizes:** text=24752, data=3408, bss=91532

**Imported libraries:**
- `loadcore` 0x0101: 6, **12** (new)
- `intrman` 0x0102: 4, 5, 6, **23** (in_irq, new)
- `stdio` 0x0102: 4
- `thbase` 0x0101: 20, 33, 35, 36, 37, 38
- **`thevent`** 0x0101: 4 (create), 5 (delete), 6 (set), 7 (iset), 8 (clear), 9 (iclear), 10 (wait) — **new library**
- `thsemap` 0x0101: 4, 5, 6, **7** (isignal, new), 8, 9
- `sysclib` 0x0101: 12, 20, 22, 27, 29, 30
- `ioman` 0x0102: 20, 21

**Exports:** 76 total (slots 0-75). Stub address: 0x5eb8.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 75 (sceCdMmode).

**Notes:** Major change: `thevent` (event flags) added, `intrman_in_irq` added. Export slot 75 (sceCdMmode) now implemented. Slots 69-74 remain stub. Slot 65 ((sceCdWM)) reverts to stub in this version.

---

### cdvdman_202 — Version 0x020d

**Sizes:** text=25360, data=3344, bss=91568

**Imported libraries:** Identical to cdvdman_165.

**Exports:** 76 total (slots 0-75). Stub address: 0x6118.

Implemented slots: same set as 165 — 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 75.

**Notes:** Minor version bump over 165; same import set and export topology. Text grew ~600 bytes.

---

### cdvdman_205 — Version 0x020e

**Sizes:** text=25888, data=3488, bss=91568

**Imported libraries:**
- `loadcore` 0x0101: 6, 12
- `intrman` 0x0102: 4, 5, 6, **17** (cpu_suspend_irq, new), **18** (cpu_resume_irq, new), 23
- `stdio` 0x0102: 4
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-10
- `thsemap` 0x0101: 4, 5, 6, 7, 8, 9
- `sysclib` 0x0101: 12, 20, 22, 27, 29, 30
- `ioman` **0x0103**: 20, 21

**Exports:** 78 total (slots 0-77). Stub address: 0x6310.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 75, 77 (sceCdStSeekF, new).

**Notes:** `intrman_cpu_suspend_irq` and `cpu_resume_irq` added. ioman bumped to 0x0103. Two new export slots: slot 76 (stub) and slot 77 (sceCdStSeekF, implemented).

---

### cdvdman_21/210 — Version 0x020e (Group A)

**Sizes:** text=25952, data=3536, bss=91568

**Imported libraries:** Same as cdvdman_205.

**Exports:** 78 total (slots 0-77). Stub address: 0x6350.

Implemented slots: same set as 205.

**Notes:** A slightly larger build than 205 (64 bytes more text). Both `cdvdman_21.irx` and `cdvdman_210.irx` are byte-identical.

---

### cdvdman_211/213/214 — Version 0x0210 (Group B)

**Sizes:** text=27712, data=3728, bss=92360

**Imported libraries:**
- **`sysmem`** 0x0101: **14** (AllocSysMemory) — **new library**
- `loadcore` 0x0101: 6, 12
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` 0x0102: 4
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-10, **11** (poll_event_flag, new)
- `thsemap` 0x0101: 4, 5, 6, 7, 8 (poll_sema dropped)
- `sysclib` 0x0101: 12, 20, 22, 27, 29, 30
- `ioman` 0x0103: 20, 21

**Exports:** 78 total (slots 0-77). Stub address: 0x6a00.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 75, 77.

**Notes:** `sysmem` introduced (AllocSysMemory for dynamic allocation). `thevent_poll_event_flag` added. `thsemap_poll_sema` removed (not needed). BSS size grows slightly (92360 vs 91568), indicating larger static buffers.

---

### cdvdman_224 — Version 0x0214

**Sizes:** text=30368, data=3792, bss=91660

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0101: 6, 12
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` 0x0102: 4
- **`dmacman`** 0x0102: 4 (ch_set_madr), 5 (ch_get_madr), 6 (ch_set_bcr), 8 (ch_set_chcr), 9 (ch_get_chcr), 14 (set_dpcr), 15 (get_dpcr) — **new library**
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-11
- `thsemap` 0x0101: 4, 5, 6, 7, 8
- `sysclib` 0x0101: **20, 22, 23** (new), **27, 29, 30** (12 dropped, 23 added)
- `ioman` **0x0104**: 20, 21

**Exports:** 79 total (slots 0-78). Stub address: 0x7414.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 74 (sceCdPowerOff, new), 75 (sceCdMmode), 77 (sceCdStSeekF), 78 (sceCdPOffCallback, new).

**Notes:** `dmacman` added — the driver now directly programs DMA channels for data transfer (DVD DMA). ioman bumped to 0x0104. Two new implemented slots: 74 (sceCdPowerOff) and 78 (sceCdPOffCallback).

---

### cdvdman_23 — Version 0x0215

**Sizes:** text=30720, data=3792, bss=91660

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` **0x0102**: 6, 12
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` **0x0103**: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-11
- `thsemap` 0x0101: 4, 5, 6, 7, 8
- `sysclib` **0x0102**: 20, 22, 23, 27, 29, 30
- `ioman` 0x0104: 20, 21

**Exports:** 79 total (slots 0-78). Stub address: 0x7574.

Implemented slots: same as 224.

**Notes:** loadcore bumped to 0x0102, stdio to 0x0103, sysclib to 0x0102. Text size 352 bytes larger than 224.

---

### cdvdman_234 — Version 0x0216

**Sizes:** text=33552, data=4528, bss=51900

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` **0x0103**: 6, 12
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-11
- `thsemap` 0x0101: 4, 5, 6, 7, 8
- `sysclib` **0x0103**: 20, 22, 23, 27, 29, 30
- `ioman` 0x0104: 20, 21

**Exports:** 82 total (slots 0-81). Stub address: 0x8080.

Implemented slots: 4-17, 21-24, 28, 29, 37, 38, 39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, **79** (sceCdReadDiskID, new), **81** (sceCdSetTimeout, new).

**Notes:** loadcore bumped to 0x0103, sysclib to 0x0103. BSS drops dramatically (51900 vs 91660 in previous) — drive buffer reorganized. Three new export slots: 79 (sceCdReadDiskID), 80 (sceCdReadGUID, stub), 81 (sceCdSetTimeout). Export 47 (sceGetFsvRbuf) address changed from 0x0074 to 0x0084 — the struct offset grew.

---

### cdvdman_241 — Version 0x0218

**Sizes:** text=35904, data=4544, bss=51920

**Imported libraries:** Same as cdvdman_234.

**Exports:** 82 total (slots 0-81). Stub address: 0x89b0.

Implemented slots: same as 234 — 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81.

**Notes:** Text grew by ~2352 bytes over 234. Same export topology.

---

### cdvdman_242 — Version 0x0219

**Sizes:** text=35904, data=4544, bss=51920

**Imported libraries:** Same as cdvdman_234/241.

**Exports:** 82 total (slots 0-81). Stub address: 0x89b0.

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81.

**Notes:** Same sizes as 241 (text=35904) but different export addresses — a distinct build. Version bumped from 0x0218 to 0x0219.

---

### cdvdman_243 — Version 0x021a

**Sizes:** text=36096, data=4544, bss=51920

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: 6, 12, **27** (new)
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0101: 20, 33, 35-38
- `thevent` 0x0101: 4-11
- `thsemap` 0x0101: 4, 5, 6, 7, 8
- `sysclib` 0x0103: 20, 22, 23, 27, 29, 30
- `ioman` 0x0104: 20, 21

**Exports:** 82 total (slots 0-81). Stub address: 0x8a70.

**Key change:** Export slot **2** is now 0x0220 (unique address) — `sceCdstm0Cb` (slot 48 = 0x2040) is still separately implemented, so slot 2 getting a unique value may represent a different sceCdstm0Cb internal variant or an init helper. All prior versions had export 2 equal to the stub.

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81. Export 2 = 0x0220 (implemented).

**Notes:** loadcore gains function 27. Export 2 first departs from the stub address — a new internal entry point is exposed.

---

### cdvdman_250 — Version 0x021c

**Sizes:** text=39520, data=5120, bss=52016

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: 6, 12, 27
- `intrman` 0x0102: 4, 5, 6, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` **0x0102**: 20, 33, 35-38
- `thevent` 0x0101: 4-11
- `thsemap` 0x0101: 4, 5, 6, 7, 8
- `sysclib` 0x0103: 20, 22, 23, 27, 29, 30
- `ioman` 0x0104: 20, 21

**Exports:** 90 total (slots 0-89). Stub address: 0x97d0. Export 2 = 0x023c (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, **81** (sceCdSetTimeout), **83** (sceCdReadDvdDualInfo, new), **84** (sceCdLayerSearchFile, new). Slots 80, 82, 85-89 are stub.

**Notes:** thbase bumped to 0x0102. Eight new export slots (82-89) added. Slots 83 (sceCdReadDvdDualInfo) and 84 (sceCdLayerSearchFile) implemented — DVD dual-layer support appears. Text grew ~3616 bytes.

---

### cdvdman_253/255 — Version 0x021d (Group C)

**Sizes:** text=40944, data=5184, bss=52016

**Imported libraries:** Same as cdvdman_250.

**Exports:** 113 total (slots 0-112). Stub address: 0x9d5c. Export 2 = 0x023c (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, **112** (sceCdApplySCmd2, new). Slots 80, 82, 85-111 are stub.

**Notes:** 23 new export slots added (89-112). The major addition is slot 112 (sceCdApplySCmd2) implemented; slots 85-111 are all stub placeholders. Both 253 and 255 are byte-identical.

---

### cdvdman_260 — Version 0x0220

**Sizes:** text=40224, data=4896, bss=35168

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: 6, 12, 27
- `intrman` 0x0102: 4, 5, 6, **7** (disable_irq, new), 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0102: 20, 33, 35-38
- `thevent` 0x0101: 4-11, **13** (refer_event_flag_status, new), **14** (irefer_event_flag_status, new)
- `thsemap` 0x0101: **6** (signal only), **7** (isignal only) — create/delete/wait all dropped
- `sysclib` 0x0103: **12** (new), **14** (new), 20, 22, 23, 27, 29, 30
- `ioman` 0x0104: 20, 21, **31** (new)

**Exports:** 123 total (slots 0-122). Stub address: 0x9a74. Export 2 = 0x024c (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, **112** (sceCdApplySCmd2), **114** (sceCdRE, new). Slots 80, 82, 85-111, 113, 115-122 are stub.

**Notes:** Significant architectural shift. `thsemap` reduced to just signal/isignal — semaphores replaced by event flags for most synchronization. `intrman_disable_irq` added. `thevent_refer/irefer_event_flag_status` added. `ioman` gains func 31. BSS drops further to 35168. Slot 114 (sceCdRE) implemented.

---

### cdvdman_270 — Version 0x0221

**Sizes:** text=40320, data=4896, bss=35168

**Imported libraries:** Identical to cdvdman_260.

**Exports:** 123 total (slots 0-122). Stub address: 0x9ad4. Export 2 = 0x024c (implemented).

Implemented slots: same as 260.

**Notes:** Minor version bump over 260. Text 96 bytes larger. Same import set and export topology.

---

### cdvdman_271/271a/271b — Version 0x0222 (Group D)

**Sizes:** text=47328, data=5712, bss=35584

**Imported libraries:**
- `sysmem` 0x0101: **4** (AllocSysMemory, new), **5** (FreeSysMemory, new), 14
- `loadcore` 0x0103: 6, 12, 27
- `intrman` 0x0102: 4, 5, 6, 7, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0102: 20, 33, 35-38
- `thevent` 0x0101: 4-11, 13, 14
- `thsemap` 0x0101: 6, 7
- `sysclib` 0x0103: 12, 14, **19** (new), 20, 22, 23, 27, 29, 30, **36** (new)
- `ioman` 0x0104: **4, 5, 6, 7, 8, 10** (open/close/read/write/ioctl/lseek — new), 20, 21, 31, **34** (new)

**Exports:** 148 total (slots 0-147). Stub address: 0xb5e8. Export 2 = 0x024c (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 44, 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, 112, 114. Slots 80, 82, 85-111, 113, 115-147 are stub.

**Notes:** `sysmem` gains alloc/free (functions 4 and 5) — full dynamic memory management. `ioman` gains a full file I/O set (open/close/read/write/ioctl/lseek and ioctl2) — the driver can now open files on IOP filesystems. `sysclib` adds functions 19 and 36. 25 new export stub slots added (123-147).

---

### cdvdman_271_2 — Version 0x0222 (Group D2)

**Sizes:** text=47248, data=5680, bss=54304

**Imported libraries:** Same set as 271/271a/271b.

**Exports:** 148 total (slots 0-147). Stub address: 0xb598. Export 2 = 0x024c (implemented).

Implemented slots: same as 271 group.

**Notes:** Same version (0x0222) as 271/271a/271b but text is 80 bytes smaller and BSS is significantly larger (54304 vs 35584). A different build variant, likely for a different hardware configuration or with different buffer sizing.

---

### cdvdman_280/280a/280b — Version 0x0223 (Group E)

**Sizes:** text=48112, data=5760, bss=54304

**Imported libraries:**
- `sysmem` 0x0101: 4, 5, 14
- `loadcore` 0x0103: 6, 12, 27
- `intrman` 0x0102: 4, 5, 6, 7, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0102: 20, 33, 35-38
- `thevent` 0x0101: 4-11, 13, 14
- `thsemap` 0x0101: 6, 7
- `sysclib` **0x0104**: 12, 14, 19, 20, 22, 23, 27, 29, 30, 36
- `ioman` 0x0104: 4-8, 10, 20, 21, 31, 34

**Exports:** 154 total (slots 0-153). Stub address: 0xb8f8. Export 2 = 0x024c (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, **40** (sceCdReadCdda, new), 44, **45** (sceCdCtrlADout, new), 46-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, 112, 114. Slots 80, 82, 85-111, 113, 115-153 are stub.

**Notes:** sysclib bumped to 0x0104. Two new implemented slots: 40 (sceCdReadCdda) and 45 (sceCdCtrlADout) — CD audio reading and audio DAC output control. Six new stub slots added (148-153).

---

### cdvdman_300/300b — Version 0x0225 (Group F)

**Sizes:** text=49504, data=5824, bss=54304

**Imported libraries:**
- `sysmem` **0x0102**: 4, 5, 14
- `loadcore` 0x0103: 6, 12, 27
- `intrman` 0x0102: 4, 5, 6, 7, 17, 18, 23
- `stdio` 0x0103: 4
- `dmacman` 0x0102: 4-6, 8-9, 14-15
- `thbase` 0x0102: 20, 33, 35-38
- `thevent` 0x0101: 4-11, 13, 14
- `thsemap` 0x0101: 6, 7
- `sysclib` 0x0104: 12, 14, 19, 20, 22, 27, 29, 30, 36 (func 23 dropped)
- `ioman` 0x0104: 4-8, 10, 20, 21, 31, 34

**Exports:** 185 total (slots 0-184). Stub address: 0xbe64/0xbfe4. Export 2 = 0x0250/0x0254 (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 40, 44, 45-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, 112, 114. Slots 80, 82, 85-111, 113, 115-184 are stub.

**Notes:** sysmem bumped to 0x0102. sysclib drops func 23. 31 new stub export slots added (154-184). `cdvdman_300.irx` and `cdvdman_300b.irx` are byte-identical (stub=0xbe64, export2=0x0250). `cdvdman_300_4.irx` is a slightly different build (text=49888, stub=0xbfe4, export2=0x0254) but same version.

---

### cdvdman_300_2/300a/310/310a/310b — Version 0x0226 (Group G)

**Sizes:** text=50464, data=6016, bss=54320

**Imported libraries:** Same as cdvdman_300/300b.

**Exports:** 191 total (slots 0-190). Stub address: 0xc21c. Export 2 = 0x0254 (implemented).

Implemented slots: 4-17, 21-24, 28, 29, 37-39, 40, 44, 45-51, 54, 56-68, 74, 75, 77, 78, 79, 81, 83, 84, 112, 114. Slots 80, 82, 85-111, 113, 115-190 are stub.

**Notes:** Final known version. Six more stub slots added (185-190) over version 0x0225. All five files in this group are byte-identical. `cdvdman_300_3.irx` is a related but distinct build at 0x0226 (text=50528, stub=0xc25c, export2=0x0258).

---

## Evolution Analysis

### Summary of Key Changes by Version

| Version | IOPRP | Key Change |
|---------|-------|------------|
| 0x0106 | 14 | Initial release; basic CD read, seek, sync, streaming |
| 0x0207 | 15 | Alarm timers added; sceCdRI/WI/ReadClock implemented; 4 new exports (62/RV/RM/WM) |
| 0x020a | 16 | sceCdReadChain, sceCdStPause, sceCdStResume added |
| 0x020b | 165 | **thevent** introduced; intrman_in_irq; isignal_sema; sceCdMmode; slot 65 (WM) reverts stub |
| 0x020d | 202 | Minor refinement |
| 0x020e | 205/21/210 | **intrman cpu_suspend/resume** added; ioman 0x0103; sceCdStSeekF (slot 77) |
| 0x0210 | 211/213/214 | **sysmem** introduced (AllocSysMemory); thevent_poll added; thsemap_poll dropped |
| 0x0214 | 224 | **dmacman** introduced (DMA channel control); sceCdPowerOff+Callback; ioman 0x0104 |
| 0x0215 | 23 | Library version bumps (loadcore 0x0102, sysclib 0x0102) |
| 0x0216 | 234 | loadcore/sysclib 0x0103; BSS halved; sceCdReadDiskID, sceCdSetTimeout; export 47 offset +0x10 |
| 0x0218 | 241 | Text growth only |
| 0x0219 | 242 | Minor build change |
| 0x021a | 243 | **Export slot 2 becomes unique** (first departure from stub pattern); loadcore func 27 |
| 0x021c | 250 | thbase 0x0102; **sceCdReadDvdDualInfo** (83), **sceCdLayerSearchFile** (84); 8 new stub slots |
| 0x021d | 253/255 | **sceCdApplySCmd2** (112) implemented; 23 new stub slots |
| 0x0220 | 260 | **thsemap reduced to signal-only** (semaphores replaced by event flags); intrman_disable_irq; thevent_refer; ioman func 31; BSS shrinks again; **sceCdRE** (114) |
| 0x0221 | 270 | Minor refinement |
| 0x0222 | 271/271_2/271a/271b | **sysmem alloc/free** (dynamic memory); **ioman gains full I/O** (open/close/read/write); sysclib funcs 19/36; 25 new stub slots |
| 0x0223 | 280/280a/280b | sysclib 0x0104; **sceCdReadCdda** (40), **sceCdCtrlADout** (45) implemented; 6 new stub slots |
| 0x0225 | 300/300b/300_4 | sysmem 0x0102; 31 new stub slots |
| 0x0226 | 300_2/300a/300_3/310/310a/310b | 6 more stub slots; final known version |

### When New Imported Libraries Were First Introduced

| Library | First IOPRP | Version | Notes |
|---------|-------------|---------|-------|
| `loadcore` | 14 | 0x0106 | Present from start |
| `intrman` | 14 | 0x0106 | Present from start |
| `stdio` | 14 | 0x0106 | Present from start |
| `thbase` | 14 | 0x0106 | Present from start |
| `thsemap` | 14 | 0x0106 | Present from start |
| `sysclib` | 14 | 0x0106 | Present from start |
| `ioman` | 14 | 0x0106 | Present from start |
| `thevent` | 165 | 0x020b | Event flags for IRQ/thread synchronization |
| `sysmem` | 211 | 0x0210 | Dynamic IOP memory allocation |
| `dmacman` | 224 | 0x0214 | DMA channel control for optical data transfer |

### When Key Import Functions Were First Added

| Function | Library | First IOPRP | Version |
|----------|---------|-------------|---------|
| thbase alarm funcs (35-38) | thbase | 15 | 0x0207 |
| loadcore func 12 | loadcore | 165 | 0x020b |
| intrman func 23 (in_irq) | intrman | 165 | 0x020b |
| thsemap isignal (7) | thsemap | 165 | 0x020b |
| intrman cpu_suspend/resume (17,18) | intrman | 205 | 0x020e |
| sysmem func 14 (AllocSysMemory) | sysmem | 211 | 0x0210 |
| thevent poll (11) | thevent | 211 | 0x0210 |
| dmacman (all 7 funcs) | dmacman | 224 | 0x0214 |
| loadcore func 27 | loadcore | 243 | 0x021a |
| intrman disable_irq (7) | intrman | 260 | 0x0220 |
| thevent refer/irefer (13,14) | thevent | 260 | 0x0220 |
| ioman func 31 | ioman | 260 | 0x0220 |
| sysmem alloc/free (4,5) | sysmem | 271 | 0x0222 |
| ioman full I/O (4-8, 10, 34) | ioman | 271 | 0x0222 |
| sysclib funcs 19, 36 | sysclib | 271 | 0x0222 |
| sysclib func 12, 14 | sysclib | 260 | 0x0220 |
