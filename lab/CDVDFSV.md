# CDVDFSV Module Analysis

This document analyzes `cdvdfsv_*.irx` (IOP module name: `cdvd_ee_driver`) extracted from IOPRP ROM images across PS2 hardware generations. Each module is the EE-side CDVD filesystem/RPC server that bridges the EE and IOP CDVD subsystems via SIF.

## Summary Table

| File | Version | Text | Data | BSS | cdvdfsv Exports | cdvdman Imports |
|------|---------|------|------|-----|-----------------|-----------------|
| 14 | 0x0105 | 9008 | 1216 | 3584 | 5 (0-4) | 22 |
| 15 | 0x0207 | 15168 | 1792 | 3840 | 5 (0-4) | 29 |
| 16 | 0x020a | 18656 | 2096 | 5392 | 5 (0-4) | 29 |
| 165 | 0x020b | 19072 | 1760 | 5600 | 6 (0-5) | 28 |
| 202 | 0x020d | 19424 | 1840 | 5600 | 6 (0-5) | 28 |
| 205 | 0x020e | 19664 | 1840 | 5600 | 6 (0-5) | 28 |
| 21 | 0x020e | 19664 | 1840 | 5600 | 6 (0-5) | 28 |
| 210 | 0x020e | 19664 | 1840 | 5600 | 6 (0-5) | 28 |
| 211 | 0x0210 | 19584 | 1840 | 5600 | 6 (0-5) | 28 |
| 213 | 0x0210 | 19584 | 1840 | 5600 | 6 (0-5) | 28 |
| 214 | 0x0210 | 19584 | 1840 | 5600 | 6 (0-5) | 28 |
| 224 | 0x0214 | 19744 | 1696 | 5584 | 6 (0-5) | 30 |
| 23 | 0x0215 | 19440 | 1680 | 5408 | 6 (0-5) | 29 |
| 234 | 0x0216 | 21568 | 1936 | 45188 | 6 (0-5) | 31 |
| 241 | 0x0218 | 21696 | 1936 | 45188 | 6 (0-5) | 31 |
| 242 | 0x0219 | 21744 | 1936 | 45188 | 6 (0-5) | 31 |
| 243 | 0x021a | 21744 | 1936 | 45188 | 6 (0-5) | 31 |
| 250 | 0x021c | 22576 | 2080 | 45188 | 6 (0-5) | 33 |
| 253 | 0x021d | 22624 | 2160 | 45188 | 6 (0-5) | 33 |
| 255 | 0x021d | 22624 | 2160 | 45188 | 6 (0-5) | 33 |
| 260 | 0x0220 | 17072 | 1952 | 4672 | 6 (0-5) | 30 |
| 270 | 0x0221 | 17072 | 1952 | 4672 | 6 (0-5) | 30 |
| 271 | 0x0222 | 17056 | 1840 | 4672 | 6 (0-5) | 30 |
| 271_2 | 0x0222 | 17072 | 1840 | 4672 | 6 (0-5) | 30 |
| 271a | 0x0222 | 17056 | 1840 | 4672 | 6 (0-5) | 30 |
| 271b | 0x0222 | 17056 | 1840 | 4672 | 6 (0-5) | 30 |
| 280 | 0x0223 | 19024 | 1840 | 4688 | 6 (0-5) | 32 |
| 280a | 0x0223 | 19024 | 1840 | 4688 | 6 (0-5) | 32 |
| 280b | 0x0223 | 19024 | 1840 | 4688 | 6 (0-5) | 32 |
| 300 | 0x0225 | 19408 | 1984 | 4688 | 6 (0-5) | 32 |
| 300_2 | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |
| 300_3 | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |
| 300_4 | 0x0225 | 19408 | 1984 | 4688 | 6 (0-5) | 32 |
| 300a | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |
| 300b | 0x0225 | 19408 | 1984 | 4688 | 6 (0-5) | 32 |
| 310 | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |
| 310a | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |
| 310b | 0x0226 | 19872 | 2016 | 4704 | 6 (0-5) | 32 |

Notes on export count: "5 (0-4)" means exports 0 through 4 (5 total); "6 (0-5)" means exports 0 through 5 (6 total). Export 0 is always `_init_module`.

---

## Per-Version Details

### cdvdfsv_14 — Version 0x0105

**Sizes:** text=9008, data=1216, bss=3584

**Imported libraries:**
- `loadcore` 0x0101: funcs 5, 6, 12
- `intrman` 0x0102: 9 (cpu_enable_irqs), 17 (cpu_suspend_irq), 18 (cpu_resume_irq)
- `stdio` 0x0102: 4 (printf)
- `thbase` 0x0101: 4 (create), 6 (start), 20 (get_id), 24 (sleep), 33 (delay)
- `sifcmd` 0x0101: 14 (init_rpc), 17 (register_rpc), 19 (set_rpc_queue), 22 (rpc_loop)
- `sifman` 0x0101: 5 (init), 7 (set_dma), 8 (dma_stat), 29 (check_init)
- `cdvdman` 0x0101: **22 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x20fc |
| 2 | 0x20fc |
| 3 | 0x20fc |
| 4 | 0x01d0 |

**Notes:** Earliest version. Minimal thbase usage (no alarm functions). No thevent. No sysclib. No sysmem. Exports 1–3 all point to the same address (stub/null handler pattern for unimplemented RPC commands). Only 5 exports.

---

### cdvdfsv_15 — Version 0x0207

**Sizes:** text=15168, data=1792, bss=3840

**Imported libraries:**
- `loadcore` 0x0101: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0102: 4
- `thbase` 0x0101: 4, 6, 20, 24, 33, 35 (set_alarm), 36 (iset_alarm), 37 (cancel_alarm), 38 (icancel_alarm)
- `sifcmd` 0x0101: 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `cdvdman` 0x0101: **29 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 22 (sceCdRI), 23 ((sceCdWI)), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 65 ((sceCdWM))

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x38bc |
| 2 | 0x38bc |
| 3 | 0x38bc |
| 4 | 0x01d0 |

**Notes:** Major expansion from v14. Text size nearly doubled (+68%). Added thbase alarm functions. Added cdvdman functions 22, 23, 24, 62, 63, 64, 65. Still no thevent, no sysclib, no sysmem. Still 5 exports, exports 1–3 still share same address.

---

### cdvdfsv_16 — Version 0x020a

**Sizes:** text=18656, data=2096, bss=5392

**Imported libraries:**
- `loadcore` 0x0101: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0102: 4
- `thbase` 0x0101: 4, 6, 20, 24, 33, 35, 36, 37, 38
- `sifcmd` 0x0101: 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `cdvdman` 0x0101: **29 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 22 (sceCdRI), 23 ((sceCdWI)), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 65 ((sceCdWM))

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x465c |
| 2 | 0x465c |
| 3 | 0x465c |
| 4 | 0x01d0 |

**Notes:** Same cdvdman import set as v15 (functions 4–65). Larger text/BSS than v15. Still no thevent, no sysclib, no sysmem. Same 5-export pattern with 1–3 sharing an address.

---

### cdvdfsv_165 — Version 0x020b

**Sizes:** text=19072, data=1760, bss=5600

**Imported libraries:**
- `loadcore` 0x0101: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18, **23 (in_irq)** ← new
- `stdio` 0x0102: 4
- `thbase` 0x0101: 4, 6, **14 (change_priority)**, **15 (ichange_priority)**, 20, 24, 33, 35, 36, 37, 38
- `thevent` 0x0101: **8 (clear_event_flag)**, **10 (wait_event_flag)** ← new library
- `sifcmd` 0x0101: 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `cdvdman` 0x0101: **28 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 75 (sceCdMmode)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x47c0 |
| 2 | 0x47c0 |
| 3 | 0x47c0 |
| 4 | 0x0204 |
| 5 | 0x0238 |

**Notes:** First version with `thevent` (event flag support). Added `intrman_in_irq` (func 23). Added `thbase_change_priority`/`ichange_priority`. **First version with 6 exports** (added export 5). cdvdman func 23 ((sceCdWI)) dropped, func 65 ((sceCdWM)) replaced by func 75 (sceCdMmode). cdvdman func 16 (sceCdPosToInt) not yet imported.

---

### cdvdfsv_202 — Version 0x020d

**Sizes:** text=19424, data=1840, bss=5600

**Imported libraries:**
- `loadcore` 0x0101: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0102: 4
- `thbase` 0x0101: 4, 6, 14, 20, **22 (refer_status)**, 24, 33, 35, 36, 37, 38
- `thevent` 0x0101: 8, 10
- `sifcmd` 0x0101: 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `sysclib` 0x0101: **29, 36** ← new library
- `cdvdman` 0x0101: **28 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 75 (sceCdMmode)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x4900 |
| 2 | 0x4900 |
| 3 | 0x4900 |
| 4 | 0x022c |
| 5 | 0x0330 |

**Notes:** First version with `sysclib` (funcs 29/36 — likely `memcpy`/`memset` or similar). Added `thbase_refer_status` (func 22). Dropped `thbase_ichange_priority`. Same cdvdman set as v165. 6 exports.

---

### cdvdfsv_205 / cdvdfsv_21 — Version 0x020e

**Sizes:** text=19664, data=1840, bss=5600
(cdvdfsv_205 and cdvdfsv_21 are **byte-for-byte identical** in metadata and imports/exports)

**Imported libraries:**
- Same as v202 (0x020d) but version incremented
- `sysclib` 0x0101: 29, 36
- `cdvdman` 0x0101: **28 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 75 (sceCdMmode)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x49f0 |
| 2 | 0x49f0 |
| 3 | 0x49f0 |
| 4 | 0x022c |
| 5 | 0x0330 |

**Notes:** Minor text size increase (+240 bytes) over v202. cdvdman imports and export count unchanged.

---

### cdvdfsv_210 / cdvdfsv_211 / cdvdfsv_213 / cdvdfsv_214 — Versions 0x020e / 0x0210

| Suffix | Version |
|--------|---------|
| 210 | 0x020e |
| 211 | 0x0210 |
| 213 | 0x0210 |
| 214 | 0x0210 |

**Sizes:** 210: text=19664, data=1840, bss=5600; 211/213/214: text=19584, data=1840, bss=5600

All four have identical imports to cdvdfsv_205 (v0x020e / v0x0210). cdvdman set is the same 28-function set. 6 exports.

**Notes:** cdvdfsv_210 shares exact metadata with cdvdfsv_205/cdvdfsv_21. cdvdfsv_211/213/214 show a slight text reduction (−80 bytes) with version bump to 0x0210.

---

### cdvdfsv_224 — Version 0x0214

**Sizes:** text=19744, data=1696, bss=5584

**Imported libraries:**
- `sysmem` 0x0101: **14** ← new library
- `loadcore` 0x0101: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0102: 4
- `thbase` 0x0101: 4, **5 (delete)**, 6, **10 (terminate)**, 14, 20, 22, 24, 33, 35, 36, 37, 38
- `thevent` 0x0101: **6 (set_event_flag)**, 8, 10
- `sifcmd` 0x0101: **12 (send_cmd)**, 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `sysclib` 0x0101: 29, 36
- `cdvdman` 0x0101: **30 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), **16 (sceCdPosToInt)**, 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), **74 (sceCdPowerOff)**, 75 (sceCdMmode)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x49f0 |
| 2 | 0x49f0 |
| 3 | 0x49f0 |
| 4 | 0x029c |
| 5 | 0x03a0 |

**Notes:** First version with `sysmem` (func 14 — AllocSysMemory). Added `thbase_delete` (5), `thbase_terminate` (10). Added `thevent_set_event_flag` (6). Added `sifcmd_send_cmd` (12). **cdvdman gained funcs 16 (sceCdPosToInt) and 74 (sceCdPowerOff)** (now 30 imports). Export addresses changed significantly.

---

### cdvdfsv_23 — Version 0x0215

**Sizes:** text=19440, data=1680, bss=5408

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0102: 5, 6, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0103: 4
- `thbase` 0x0101: 4, 5, 6, **8 (exit)**, 10, 14, 20, 22, 33, 35, 36, 37, 38
- `thevent` 0x0101: 6, 8, 10
- `sifcmd` 0x0101: 12, 14, 17, 19, 22
- `sifman` 0x0101: 5, 7, 8, 29
- `sysclib` 0x0102: 29, 36
- `cdvdman` 0x0101: **29 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 21 (sceCdCheckCmd), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 74 (sceCdPowerOff), 75 (sceCdMmode)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0000 (_init_module) |
| 1 | 0x48c0 |
| 2 | 0x48c0 |
| 3 | 0x48c0 |
| 4 | 0x029c |
| 5 | 0x03a0 |

**Notes:** Added `thbase_exit` (8). Library version numbers incremented (loadcore 0x0102, stdio 0x0103, sysclib 0x0102). **cdvdman lost func 22 (sceCdRI) compared to v224** (29 vs 30 imports). Funcs 16 (sceCdPosToInt), 74 (sceCdPowerOff), 75 (sceCdMmode) retained.

---

### cdvdfsv_234 — Version 0x0216

**Sizes:** text=21568, data=1936, bss=**45188**

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: 5, 6, **7**, 12
- `intrman` 0x0102: 9, 17, 18
- `stdio` 0x0103: 4
- `thbase` 0x0101: 4, 5, 6, **9 (exit_delete)**, 10, 14, 20, 22, 33, 35, 36, 37, 38
- `thevent` 0x0101: 8, 10
- `sifcmd` 0x0101: 12, 14, 17, 19, 22, **24 (remove_rpc)**, **25 (remove_rpc_queue)**
- `sifman` 0x0101: 5, 7, 8, 29
- `sysclib` 0x0103: 29, 36
- `cdvdman` 0x0101: **31 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 74 (sceCdPowerOff), 75 (sceCdMmode), **81 (sceCdSetTimeout)**

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x00d8 (_init_module) |
| 1 | 0x50f0 |
| 2 | 0x50f0 |
| 3 | 0x50f0 |
| 4 | 0x03a0 |
| 5 | 0x04a4 |

**Notes:** **BSS size exploded from ~5KB to 45188 bytes** — major internal buffer allocation. text grew by ~2KB. Added `loadcore` func 7. Replaced `thbase_exit` (8) with `thbase_exit_delete` (9). Dropped `thevent_set_event_flag` (6). Added `sifcmd_remove_rpc` (24) and `remove_rpc_queue` (25). **cdvdman gained func 81 (sceCdSetTimeout)** (now 31 imports). Re-added cdvdman func 22 (sceCdRI) (dropped in v23). `_init_module` entry addr changed to 0x00d8 (non-zero).

---

### cdvdfsv_241 — Version 0x0218

**Sizes:** text=21696, data=1936, bss=45188

Identical imports to v234 (31 cdvdman functions). 6 exports.

**Notes:** Minor text size increase (+128 bytes). Same BSS, same cdvdman set.

---

### cdvdfsv_242 — Version 0x0219

**Sizes:** text=21744, data=1936, bss=45188

Identical imports to v234/v241. 6 exports.

**Notes:** +48 bytes text vs v241.

---

### cdvdfsv_243 — Version 0x021a

**Sizes:** text=21744, data=1936, bss=45188

Identical to v242 in all metadata and imports. 6 exports.

**Notes:** Same binary content as v242 with version bump to 0x021a.

---

### cdvdfsv_250 — Version 0x021c

**Sizes:** text=22576, data=2080, bss=45188

**Imported libraries:**
- Same as v234–v243 except:
  - `thbase` 0x0102 (version bump)
  - `cdvdman` 0x0101: **33 functions** — 4 (sceCdInit), 5 (sceCdStandby), 6 (sceCdRead), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 21 (sceCdCheckCmd), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 46 (sceCdNop), 47 (sceGetFsvRbuf), 49 (sceCdstm1Cb), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 74 (sceCdPowerOff), 75 (sceCdMmode), 81 (sceCdSetTimeout), **83 (sceCdReadDvdDualInfo), 84 (sceCdLayerSearchFile)**

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x00d8 (_init_module) |
| 1 | 0x54d0 |
| 2 | 0x54d0 |
| 3 | 0x54d0 |
| 4 | 0x03b0 |
| 5 | 0x04d4 |

**Notes:** **cdvdman gained funcs 83 (sceCdReadDvdDualInfo) and 84 (sceCdLayerSearchFile)** (now 33 imports — the largest count up to this point). Text size grew by ~1KB. `thbase` library version bumped to 0x0102.

---

### cdvdfsv_253 / cdvdfsv_255 — Version 0x021d

**Sizes:** text=22624, data=2160, bss=45188
(253 and 255 are identical in metadata)

Identical cdvdman import set to v250 (33 functions: 4–84 with 83, 84 included). 6 exports.

**Notes:** +48 bytes text, +80 bytes data vs v250.

---

### cdvdfsv_260 — Version 0x0220

**Sizes:** text=17072, data=1952, bss=**4672**

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: **6, 7, 11, 12** (changed — dropped func 5, added func 11)
- `intrman` 0x0102: **7 (disable_irq)** ← replaced cpu_enable_irqs with disable_irq; 17, 18
- `stdio` 0x0103: 4
- `dmacman` 0x0102: **4 (ch_set_madr), 6 (ch_set_bcr), 8 (ch_set_chcr), 9 (ch_get_chcr)** ← new library
- `thbase` 0x0102: 4, 5, 6, 9, 10, 14, 20, 22, 33, 35, **37** (dropped 36/iset_alarm)
- `thevent` 0x0101: **7 (iset_event_flag)**, 8, 10
- `sifcmd` 0x0101: 12, 14, 17, 19, 22, 24, 25
- `sifman` 0x0101: 5, 7, 8, 29
- `sysclib` 0x0103: 29, 36
- `ioman` 0x0104: **31** ← new library
- `cdvdman` 0x0101: **30 functions** — 4 (sceCdInit), 5 (sceCdStandby), **7 (sceCdSeek)**, 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), 44 (sceCdGetReadPos), 47 (sceGetFsvRbuf), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 74 (sceCdPowerOff), 75 (sceCdMmode), 81 (sceCdSetTimeout), 83 (sceCdReadDvdDualInfo), 84 (sceCdLayerSearchFile), **114 (sceCdRE)**

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0100 (_init_module) |
| 1 | 0x3f10 |
| 2 | 0x3f10 |
| 3 | 0x3f10 |
| 4 | 0x047c |
| 5 | 0x05a0 |

**Notes:** **Major architectural change.** BSS dropped from 45KB back to ~4.6KB. Text shrank by ~5.5KB (from 22KB to 17KB). Added `dmacman` (DMA controller) — first use of direct DMAC access. Added `ioman` func 31. Switched to `thevent_iset_event_flag` (7) instead of regular set (6). Dropped `thbase_iset_alarm` (36). **cdvdman func 6 (sceCdRead) dropped**, func 7 (sceCdSeek) kept (was both before). **cdvdman func 21 (sceCdCheckCmd), 46 (sceCdNop), 49 (sceCdstm1Cb) dropped. Func 114 (sceCdRE) added** (30 imports, down from 33). `_init_module` entry addr changed to 0x0100.

---

### cdvdfsv_270 — Version 0x0221

**Sizes:** text=17072, data=1952, bss=4672

Identical to v260 in all metadata and imports. 6 exports.

**Notes:** Pure version bump from 0x0220 to 0x0221 with no structural changes.

---

### cdvdfsv_271 / cdvdfsv_271_2 / cdvdfsv_271a / cdvdfsv_271b — Version 0x0222

| Suffix | text | data |
|--------|------|------|
| 271 | 17056 | 1840 |
| 271_2 | 17072 | 1840 |
| 271a | 17056 | 1840 |
| 271b | 17056 | 1840 |

All four have identical imports to v260/v270 (30 cdvdman functions). 6 exports.

**Notes:** `271` / `271a` / `271b` are 16 bytes smaller than `271_2` and v260/v270 in text. data dropped by 112 bytes versus v260. The three sub-variants (271, 271a, 271b) appear to be the same binary; `271_2` has a slightly different text size.

---

### cdvdfsv_280 / cdvdfsv_280a / cdvdfsv_280b — Version 0x0223

**Sizes:** text=19024, data=1840, bss=4688

**Imported libraries:**
- Same as v271 except:
- `sysclib` 0x0104 (version bump)
- `cdvdman` 0x0101: **32 functions** — 4 (sceCdInit), 5 (sceCdStandby), 7 (sceCdSeek), 8 (sceCdGetError), 9 (sceCdGetToc), 10 (sceCdSearchFile), 11 (sceCdSync), 12 (sceCdGetDiskType), 14 (sceCdTrayReq), 15 (sceCdStop), 16 (sceCdPosToInt), 22 (sceCdRI), 24 (sceCdReadClock), 28 (sceCdStatus), 29 (sceCdApplySCmd), 38 (sceCdPause), 39 (sceCdBreak), **40 (sceCdReadCdda)**, 44 (sceCdGetReadPos), **45 (sceCdCtrlADout)**, 47 (sceGetFsvRbuf), 50 (sceCdSC), 54 (sceCdApplyNCmd), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 74 (sceCdPowerOff), 75 (sceCdMmode), 81 (sceCdSetTimeout), 83 (sceCdReadDvdDualInfo), 84 (sceCdLayerSearchFile), 114 (sceCdRE)

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0100 (_init_module) |
| 1 | 0x46a0 |
| 2 | 0x46a0 |
| 3 | 0x46a0 |
| 4 | 0x047c |
| 5 | 0x05a0 |

**Notes:** **cdvdman gained funcs 40 (sceCdReadCdda) and 45 (sceCdCtrlADout)** (now 32 imports). Text grew by ~2KB. sysclib version bumped to 0x0104. All three 280 variants are identical.

---

### cdvdfsv_300 / cdvdfsv_300_4 / cdvdfsv_300b — Version 0x0225

**Sizes:** text=19408, data=1984, bss=4688

Identical cdvdman imports to v280 (32 functions). 6 exports.

**Notes:** Minor text size increase. Slight data size increase. Same cdvdman set as v280 series. sysmem bumped to 0x0102.

---

### cdvdfsv_300_2 / cdvdfsv_300_3 / cdvdfsv_300a / cdvdfsv_310 / cdvdfsv_310a / cdvdfsv_310b — Version 0x0226

**Sizes:** text=19872, data=2016, bss=4704

**Imported libraries:**
- Same as v300 (0x0225) except:
- `intrman` 0x0102: 7, 17, 18, **23 (in_irq)** ← re-added
- `_init_module` entry addr: 0x0158 (for 300_2/300_3/300a/310/310a/310b) vs 0x0100 (for 300_4/300b)
- `cdvdman` 0x0101: same 32-function set as v300 series

**Exported cdvdfsv library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x0158 (_init_module) — for 300_2/300_3/300a/310/310a/310b |
| 1 | 0x49f0 |
| 2 | 0x49f0 |
| 3 | 0x49f0 |
| 4 | 0x04d4 |
| 5 | 0x05f8 |

**Notes:** Re-added `intrman_in_irq` (func 23) last seen in v165. `_init_module` entry address changed to 0x0158 for the "a" variants. The "b" variants (300b) keep version 0x0225 and entry 0x0100 — they are the same binary as v300/300_4. The "a" variants all share the same binary. cdvdman import set unchanged from v280/v300.

---

## Evolution Analysis

### Version Progression Summary

The version field in the module increments through: 0x0105 → 0x0207 → 0x020a → 0x020b → 0x020d → 0x020e → 0x0210 → 0x0214 → 0x0215 → 0x0216 → 0x0218 → 0x0219 → 0x021a → 0x021c → 0x021d → 0x0220 → 0x0221 → 0x0222 → 0x0223 → 0x0225 → 0x0226

### cdvdman Import Function Evolution

The table below shows when each cdvdman function number was first imported and whether it was ever dropped:

| Func# | Name | First seen | Last seen | Notes |
|-------|------|-----------|-----------|-------|
| 4 | sceCdInit | 14 (0x0105) | all | Always present |
| 5 | sceCdStandby | 14 | all | Always present |
| 6 | sceCdRead | 14 | 255 (0x021d) | Dropped at 260 (0x0220) |
| 7 | sceCdSeek | 14 | all | Always present |
| 8 | sceCdGetError | 14 | all | Always present |
| 9 | sceCdGetToc | 14 | all | Always present |
| 10 | sceCdSearchFile | 14 | all | Always present |
| 11 | sceCdSync | 14 | all | Always present |
| 12 | sceCdGetDiskType | 14 | all | Always present |
| 14 | sceCdTrayReq | 14 | all | Always present |
| 15 | sceCdStop | 14 | all | Always present |
| 16 | sceCdPosToInt | 224 (0x0214) | all | Added in IOPRP224 |
| 21 | sceCdCheckCmd | 14 | 255 (0x021d) | Dropped at 260 (0x0220) |
| 22 | sceCdRI | 15 (0x0207) | 165 (0x020b) then 23 then 234 | Dropped in v165 (0x020b), reappeared in v23 (0x0215), then again in v234+ |
| 23 | (sceCdWI) | 15 (0x0207) | 16 (0x020a) | Dropped in 165 and not re-added |
| 24 | sceCdReadClock | 15 (0x0207) | all | Always present from v15 |
| 28 | sceCdStatus | 14 | all | Always present |
| 29 | sceCdApplySCmd | 14 | all | Always present |
| 38 | sceCdPause | 14 | all | Always present |
| 39 | sceCdBreak | 14 | all | Always present |
| 40 | sceCdReadCdda | 280 (0x0223) | all | Added in IOPRP280 |
| 44 | sceCdGetReadPos | 14 | all | Always present |
| 45 | sceCdCtrlADout | 280 (0x0223) | all | Added in IOPRP280 |
| 46 | sceCdNop | 14 | 255 (0x021d) | Dropped at 260 |
| 47 | sceGetFsvRbuf | 14 | all | Always present |
| 49 | sceCdstm1Cb | 14 | 255 (0x021d) | Dropped at 260 |
| 50 | sceCdSC | 14 | all | Always present |
| 54 | sceCdApplyNCmd | 14 | all | Always present |
| 62 | sceCdRead0 | 15 (0x0207) | all | Added v15, always present |
| 63 | sceCdRV | 15 (0x0207) | all | Added v15, always present |
| 64 | sceCdRM | 15 (0x0207) | all | Added v15, always present |
| 65 | (sceCdWM) | 15 (0x0207) | 16 (0x020a) | Dropped in 165, never returned |
| 74 | sceCdPowerOff | 224 (0x0214) | all | Added in IOPRP224 |
| 75 | sceCdMmode | 165 (0x020b) | all | Added in IOPRP165 |
| 81 | sceCdSetTimeout | 234 (0x0216) | all | Added in IOPRP234 |
| 83 | sceCdReadDvdDualInfo | 250 (0x021c) | all | Added in IOPRP250 |
| 84 | sceCdLayerSearchFile | 250 (0x021c) | all | Added in IOPRP250 |
| 114 | sceCdRE | 260 (0x0220) | all | Added in IOPRP260, SlimLine era |

### Key Evolutionary Transitions

**Transition 1: v14 → v15 (0x0105 → 0x0207)**
Largest single-version expansion. Text nearly doubled. Added alarm functions to thbase. Added cdvdman funcs 22 (sceCdRI), 23 ((sceCdWI)), 24 (sceCdReadClock), 62 (sceCdRead0), 63 (sceCdRV), 64 (sceCdRM), 65 ((sceCdWM)) (7 new functions). This corresponds to adding full CD-ROM async completion handling.

**Transition 2: v16 → v165 (0x020a → 0x020b)**
Introduction of `thevent` (event flags). Added `intrman_in_irq`. First version with 6 exports (export 5 added). cdvdman dropped func 23 ((sceCdWI)), 65 ((sceCdWM)); added func 75 (sceCdMmode).

**Transition 3: v165 → v202 (0x020b → 0x020d)**
Introduction of `sysclib` (string/memory utilities). Added `thbase_refer_status`. This marks the beginning of the mature phase.

**Transition 4: v214 → v224 (0x0210 → 0x0214)**
Introduction of `sysmem` (direct kernel memory allocation). Added `thbase_delete/terminate`. Added `sifcmd_send_cmd`. Added `thevent_set_event_flag`. **cdvdman gained funcs 16 (sceCdPosToInt) and 74 (sceCdPowerOff)** — first major cdvdman expansion since v15.

**Transition 5: v23 → v234 (0x0215 → 0x0216)**
**BSS size exploded from ~5.4KB to 45.2KB** — a 8× increase indicating large internal static buffers were added (likely streaming/DMA ring buffers). Added `sifcmd_remove_rpc/remove_rpc_queue` (proper cleanup). Added `loadcore` func 7. Changed from `thbase_exit` to `thbase_exit_delete`. **cdvdman gained func 81 (sceCdSetTimeout).** The non-zero `_init_module` entry first appears here (0x00d8).

**Transition 6: v255 → v260 (0x021d → 0x0220)**
**Biggest architectural overhaul.** BSS returned to ~4.6KB (dropped from 45KB). Text shrank by ~5.5KB. Added `dmacman` (direct DMA channel control — suggests the module now handles DMA itself rather than delegating to cdvdman). Added `ioman` func 31. Changed `intrman` from `cpu_enable_irqs` to `disable_irq`. Changed `thevent` from `set_event_flag` to `iset_event_flag` (interrupt-safe version). **cdvdman lost funcs 6 (sceCdRead), 21 (sceCdCheckCmd), 46 (sceCdNop), 49 (sceCdstm1Cb); gained func 114 (sceCdRE).** Net cdvdman count dropped from 33 to 30. This corresponds to the SlimLine PS2 (PSTwo) hardware redesign.

**Transition 7: v271 → v280 (0x0222 → 0x0223)**
**cdvdman gained funcs 40 (sceCdReadCdda) and 45 (sceCdCtrlADout)** (32 imports). Minor text increase.

**Transition 8: v300 → v300_2/300a/310 (0x0225 → 0x0226)**
Re-added `intrman_in_irq` (func 23). `_init_module` entry changed to 0x0158. Minor size increase.

### Export Count Changes

| Versions | Exports | Change |
|----------|---------|--------|
| 14, 15, 16 | 5 (indices 0–4) | Baseline |
| 165 onward | 6 (indices 0–5) | +1 export added |

Export indices 1, 2, and 3 always point to the same address across all versions — these appear to be a stub pattern where three RPC command slots share the same (possibly no-op or error) handler. Export 0 is always `_init_module`. Export 4 and Export 5 are the active RPC dispatch handlers.

### Notable Identical Binaries

Several IOPRP images contain identical cdvdfsv binaries (same version, sizes, and import/export tables):

| Group | Files | Version |
|-------|-------|---------|
| A | 205, 21, 210 | 0x020e |
| B | 211, 213, 214 | 0x0210 |
| C | 253, 255 | 0x021d |
| D | 260, 270 | 0x0220/0x0221 |
| E | 271, 271a, 271b | 0x0222 |
| F | 280, 280a, 280b | 0x0223 |
| G | 300, 300_4, 300b | 0x0225 |
| H | 300_2, 300_3, 300a, 310, 310a, 310b | 0x0226 |

This reflects Sony's practice of shipping the same IOPRP module across multiple hardware revisions or regional variants.

---

## cdvdman Function Reference

Complete export slot → function name mapping for `cdvdman`, derived from `cdvdman_emu/exports.tab` and the ps2sdk `exports.tab`. Names in parentheses are stubs/dummies in open-source implementations; the slot number is still correct.

| Slot | Name |
|------|------|
| 0 | _start |
| 1 | _retonly |
| 2 | _shutdown |
| 3 | _retonly |
| 4 | sceCdInit |
| 5 | sceCdStandby |
| 6 | sceCdRead |
| 7 | sceCdSeek |
| 8 | sceCdGetError |
| 9 | sceCdGetToc |
| 10 | sceCdSearchFile |
| 11 | sceCdSync |
| 12 | sceCdGetDiskType |
| 13 | sceCdDiskReady |
| 14 | sceCdTrayReq |
| 15 | sceCdStop |
| 16 | sceCdPosToInt |
| 17 | sceCdIntToPos |
| 18 | (unknown) |
| 19 | (unknown) |
| 20 | (unknown) |
| 21 | sceCdCheckCmd |
| 22 | sceCdRI |
| 23 | (sceCdWI) |
| 24 | sceCdReadClock |
| 25 | (sceCdWriteClock) |
| 26 | (sceCdReadNVM) |
| 27 | (sceCdWriteNVM) |
| 28 | sceCdStatus |
| 29 | sceCdApplySCmd |
| 30 | (sceCdSetHDMode) |
| 31 | (sceCdOpenConfig) |
| 32 | (sceCdCloseConfig) |
| 33 | (sceCdReadConfig) |
| 34 | (sceCdWriteConfig) |
| 35 | (sceCdReadKey) |
| 36 | (sceCdDecSet) |
| 37 | sceCdCallback |
| 38 | sceCdPause |
| 39 | sceCdBreak |
| 40 | sceCdReadCdda |
| 41 | (sceCdReadConsoleID) |
| 42 | (sceCdWriteConsoleID) |
| 43 | (sceCdMV) |
| 44 | sceCdGetReadPos |
| 45 | sceCdCtrlADout |
| 46 | sceCdNop |
| 47 | sceGetFsvRbuf |
| 48 | sceCdstm0Cb |
| 49 | sceCdstm1Cb |
| 50 | sceCdSC |
| 51 | sceCdRC |
| 52 | (sceCdForbidDVDP) |
| 53 | (sceCdReadSUBQ) |
| 54 | sceCdApplyNCmd |
| 55 | (sceCdAutoAdjustCtrl) |
| 56 | sceCdStInit |
| 57 | sceCdStRead |
| 58 | sceCdStSeek |
| 59 | sceCdStStart |
| 60 | sceCdStStat |
| 61 | sceCdStStop |
| 62 | sceCdRead0 |
| 63 | sceCdRV |
| 64 | sceCdRM |
| 65 | (sceCdWM) |
| 66 | sceCdReadChain |
| 67 | sceCdStPause |
| 68 | sceCdStResume |
| 69 | (sceCdForbidRead) |
| 70 | (sceCdBootCertify) |
| 71 | (sceCdSpinCtrlIOP) |
| 72 | (sceCdBlueLEDCtl) |
| 73 | (sceCdCancelPOffRdy) |
| 74 | sceCdPowerOff |
| 75 | sceCdMmode |
| 76 | (sceCdReadFull) |
| 77 | sceCdStSeekF |
| 78 | sceCdPOffCallback |
| 79 | sceCdReadDiskID |
| 80 | sceCdReadGUID |
| 81 | sceCdSetTimeout |
| 82 | sceCdReadModelID |
| 83 | sceCdReadDvdDualInfo |
| 84 | sceCdLayerSearchFile |
| 85–111 | (unknown) |
| 112 | sceCdApplySCmd2 |
| 113 | (unknown) |
| 114 | sceCdRE |
