# CDVDSTM Module Analysis

This document analyzes `cdvdstm_*.irx` (IOP module name: `cdvd_st_driver`) — the IOP-side CDVD streaming driver. It handles sector streaming from CD/DVD directly to EE memory, used by games that stream audio or video off disc without going through the filesystem.

No CDVDSTM module is embedded in the IOPRP images; these are standalone IRX files bundled separately by the bootloader.

---

## Summary Table

| File   | Version | Text  | Data | BSS  | cdvdstm Exports | cdvdman Imports |
|--------|---------|-------|------|------|-----------------|-----------------|
| 242    | 0x0202  | 17568 | 1264 | 1168 | 5 (0-4)         | 10              |
| 253    | 0x0201  | 12528 | 1168 | 1152 | 5 (0-4)         | 9               |
| 255    | 0x0201  | 12528 | 1168 | 1152 | 5 (0-4)         | 9               |
| 260    | 0x0201  | 12528 | 1168 | 1152 | 5 (0-4)         | 9               |
| 270    | 0x0202  | 17568 | 1264 | 1168 | 5 (0-4)         | 10              |
| 271a   | 0x0201  | 12528 | 1168 | 1152 | 5 (0-4)         | 9               |
| 271b   | 0x0202  | 17568 | 1264 | 1168 | 5 (0-4)         | 10              |
| 280a   | 0x0201  | 12528 | 1168 | 1152 | 5 (0-4)         | 9               |
| 280b   | 0x0202  | 17568 | 1264 | 1168 | 5 (0-4)         | 10              |
| 300a   | 0x0202  | 17024 | 1264 | 1168 | 5 (0-4)         | 10              |
| 300b   | 0x0202  | 17024 | 1264 | 1168 | 5 (0-4)         | 10              |
| 310a   | 0x0202  | 17024 | 1264 | 1168 | 5 (0-4)         | 10              |
| 310b   | 0x0202  | 17024 | 1264 | 1168 | 5 (0-4)         | 10              |

All modules share the same BSS size within their variant class. Export count is always 5 (0–4); export 0 is always `_init_module`.

---

## Variant Groups

There are **3 distinct binaries** across 13 files:

| Variant | Module ver | Text  | Files                                        | sceCdReadCdda |
|---------|-----------|-------|----------------------------------------------|---------------|
| A       | 0x0201    | 12528 | 253, 255, 260, 271a, 280a                    | No            |
| B       | 0x0202    | 17568 | 242, 270, 271b, 280b                         | Yes           |
| C       | 0x0202    | 17024 | 300a, 300b, 310a, 310b                       | Yes           |

Within variant A: files 253/255/260 have entry=0x16b8; files 271a/280a have entry=0x16bc — a 4-byte difference, likely one added/changed instruction. The exported function addresses are otherwise identical.

Variants B and C share the same import set; C is ~544 bytes smaller in text, reflecting a minor code-size optimization introduced around IOPRP300.

---

## Per-Variant Details

### Variant A — Version 0x0201 (text=12528)
*Files: cdvdstm_253, cdvdstm_255, cdvdstm_260, cdvdstm_271a, cdvdstm_280a*

**Imported libraries:**
- `sysmem` 0x0101: 14
- `loadcore` 0x0103: 6, 7
- `intrman` 0x0102: 17 (cpu_suspend_irq), 18 (cpu_resume_irq), 23 (in_irq)
- `stdio` 0x0103: 4 (printf)
- `thbase` 0x0102: 33 (delay), 36 (iset_alarm), 37 (cancel_alarm), 38 (icancel_alarm)
- `thevent` 0x0101: 6 (set_event_flag), 7 (iset_event_flag), 8 (clear_event_flag), 9 (iclear_event_flag), 10 (wait_event_flag)
- `thsemap` 0x0101: 4 (create_sema), 5 (delete_sema), 6 (signal_sema), 8 (wait_sema)
- `sifman` 0x0101: 7 (set_dma), 8 (dma_stat)
- `ioman` 0x0104: 20, 21
- `cdvdman` 0x0101: **9 functions** — 11 (sceCdSync), 12 (sceCdGetDiskType), 39 (sceCdBreak), 46 (sceCdNop), 48 (sceCdstm0Cb), 49 (sceCdstm1Cb), 50 (sceCdSC), 61 (sceCdStStop), 114 (sceCdRE)

**Exported cdvdstm library (0x0101):**
| # | Address (253/255/260) | Address (271a/280a) |
|---|----------------------|---------------------|
| 0 | 0x16b8 (_init_module) | 0x16bc (_init_module) |
| 1 | 0x2ebc               | 0x2ebc              |
| 2 | 0x0124               | 0x0124              |
| 3 | 0x2ebc               | 0x2ebc              |
| 4 | 0x00f0               | 0x00f0              |

Exports 1 and 3 share the same address (stub/alias pattern). Export 2 and 4 are fixed low-address stubs.

---

### Variant B — Version 0x0202 (text=17568)
*Files: cdvdstm_242, cdvdstm_270, cdvdstm_271b, cdvdstm_280b*

**Imported libraries:** identical to Variant A, plus one additional cdvdman function:
- `cdvdman` 0x0101: **10 functions** — 11 (sceCdSync), 12 (sceCdGetDiskType), 39 (sceCdBreak), **40 (sceCdReadCdda)**, 46 (sceCdNop), 48 (sceCdstm0Cb), 49 (sceCdstm1Cb), 50 (sceCdSC), 61 (sceCdStStop), 114 (sceCdRE)

All other library imports are identical to Variant A.

**Exported cdvdstm library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x16e8 (_init_module) |
| 1 | 0x425c |
| 2 | 0x0124 |
| 3 | 0x425c |
| 4 | 0x00f0 |

---

### Variant C — Version 0x0202 (text=17024)
*Files: cdvdstm_300a, cdvdstm_300b, cdvdstm_310a, cdvdstm_310b*

Imports are identical to Variant B (same 10 cdvdman functions including sceCdReadCdda).

**Exported cdvdstm library (0x0101):**
| # | Address |
|---|---------|
| 0 | 0x15dc (_init_module) |
| 1 | 0x403c |
| 2 | 0x0124 |
| 3 | 0x403c |
| 4 | 0x00f0 |

Text shrunk by 544 bytes vs Variant B with no functional change — likely dead-code elimination or minor refactoring in IOPRP300.

---

## a/b Suffix Meaning

From IOPRP271 onward, two cdvdstm variants are shipped together:

| Suffix | Variant | sceCdReadCdda | Likely target |
|--------|---------|---------------|---------------|
| a      | A / 0x0201 | No         | Slim models / DVD-only streaming |
| b      | B or C / 0x0202 | Yes     | Fat models / full audio CD streaming |

cdvdstm_242 (no suffix) is already the full 0x0202 variant, matching the fat PS2 era before slim hardware existed.

---

## cdvdman Functions Used

| Slot | Name            | Variant A | Variant B/C |
|------|-----------------|-----------|-------------|
| 11   | sceCdSync       | ✓         | ✓           |
| 12   | sceCdGetDiskType| ✓         | ✓           |
| 39   | sceCdBreak      | ✓         | ✓           |
| 40   | sceCdReadCdda   | —         | ✓           |
| 46   | sceCdNop        | ✓         | ✓           |
| 48   | sceCdstm0Cb     | ✓         | ✓           |
| 49   | sceCdstm1Cb     | ✓         | ✓           |
| 50   | sceCdSC         | ✓         | ✓           |
| 61   | sceCdStStop     | ✓         | ✓           |
| 114  | sceCdRE         | ✓         | ✓           |

`sceCdRE` (slot 114) is the late-added function that enables reading of encrypted sectors (used for copy protection checks). It is imported by all variants — including the earliest cdvdstm_242 — suggesting it was present from the moment streaming was introduced.

`sceCdReadCdda` (slot 40) is the only differentiator between variants: Variant A omits it entirely, making Variant A a DVD-only streaming driver.
