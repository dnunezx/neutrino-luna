# CDVD Emulation Groups

Cross-referencing the behavioral findings (RESULTS.md) with the module analysis (CDVDMAN.md, CDVDFSV.md, CDVDSTM.md) gives **5 groups** with distinct emulation requirements.

---

## Group 0 — "Pre-v2" · bios_baseline, ioprp14 (cdvdman ≤v0x0106)

| Criterion | Value |
|-----------|-------|
| sceCdMmode | **not implemented** (returns 0/18) |
| sceCdRead no-disc | returns **1**, error via callback |
| Concurrent reads | **second accepted** (queued) |
| Streaming | cdvdman, hardcoded 16-sector reads, **continuous** (Era 0) |
| sceCdStStop reason | always 1 — no reason=8 |
| cdvdman exports | 62 slots |
| cdvdstm.irx needed | No |
| Key missing functions | sceCdMmode, sceCdReadChain, StPause, StResume, StSeekF, RI/WI/ReadClock |

**Neutrino concern:** callback fires **continuously** with each CDRead completion — no stopping condition. Games targeting this era are unlikely on neutrino, but the bios_baseline is what runs when no IOPRP is loaded. `sceCdStStart` returns 0 on bios_baseline without mmode.

**Representative:** `ioprp_14.img`

---

## Group 1 — "Classic v2" · ioprp15–16 (v0x0207–v0x020a)

| Criterion | Value |
|-----------|-------|
| sceCdMmode | **not implemented** |
| sceCdRead no-disc | returns **1**, error via callback |
| Concurrent reads | second **accepted** |
| Streaming | cdvdman, 16-sector reads, **fixed count** (likely Era 1-like) |
| sceCdStStop reason | always 1 |
| cdvdman exports | 66–69 slots |
| cdvdstm.irx needed | No |
| Added vs Group 0 | sceCdRI/WI/ReadClock (22-24), Read0/RV/RM/WM (62-65), ReadChain (66), StPause/StResume (67-68) |

**Neutrino concern:** same no-mmode path as Group 0, but ioprp15/16 show 8 callbacks (fixed) vs bios/ioprp14's continuously-growing count — streaming behavior boundary is between ioprp14 and ioprp15.

**Representatives:** `ioprp_15.img`, `ioprp_16.img`

---

## Group 2 — "mmode era" · ioprp165–214 (v0x020b–v0x0210)

| Criterion | Value |
|-----------|-------|
| sceCdMmode | **works** (returns 1) |
| sceCdRead no-disc | returns **0**, silent (err=0) |
| Concurrent reads | second **rejected** |
| Streaming | cdvdman, bank-aware CDReads |
| sceCdStStop reason | **none** (ioprp165 only) or **reason=8** (ioprp202–214, Era 2) |
| cdvdman exports | 76–78 slots |
| cdvdstm.irx needed | No |
| Added vs Group 1 | sceCdMmode (75), thevent, sceCdStSeekF (77) |

**Internal split:**
- **2a — ioprp165 only (Era 1):** StStop fires no reason=8
- **2b — ioprp202–214 (Era 2):** StStop always fires final reason=8 (`SCECdFuncBreak`)

**Neutrino concern:** the reason=8 from StStop is the first significant behavioral split in streaming. `cdvdman_emu` must track which era it's targeting. Also: no dmacman here — neutrino must not depend on DMA-era cdvdman behavior for this group.

**Representatives:** `ioprp_165.img`, `ioprp_202.img`, `ioprp_211.img`

---

## Group 3 — "DMA era" · ioprp224–255 (v0x0214–v0x021d)

| Criterion | Value |
|-----------|-------|
| sceCdMmode | works |
| sceCdRead no-disc | returns **0**, explicit **err=254** |
| Concurrent reads | rejected |
| Streaming | cdvdman, **cb_count = bankmax exactly** (Era 3) |
| sceCdStStop reason | **timing-dependent** (only on overlap) |
| sceCdRE (114) | **not yet** |
| DVD dual-layer | ioprp250+ only (sceCdReadDvdDualInfo/LayerSearchFile) |
| cdvdman exports | 79–113 slots |
| cdvdstm.irx needed | No |

**Internal split:**
- **3a — ioprp224–23 (v0x0214–v0x0215):** sceCdSearchFile broken in PCSX2 (mmode_ret=1 but search_ret=0); streaming still Era 2 (reason=8 from StStop)
- **3b — ioprp234–243 (v0x0216–v0x021a):** BSS halved (91KB→52KB); streaming Era 3; sceCdReadDiskID (79), sceCdSetTimeout (81)
- **3c — ioprp250–255 (v0x021c–v0x021d):** DVD dual-layer (sceCdReadDvdDualInfo slot 83, sceCdLayerSearchFile slot 84); sceCdApplySCmd2 (112)

**Neutrino concern:** Era 3 streaming is the target behavior (cb_count=bankmax, one CDRead per bank on start). The broken search in 3a is PCSX2-specific. The sceCdApplySCmd2 addition matters for copy-protection flows.

**Representatives:** `ioprp_234.img`, `ioprp_250.img`, `ioprp_253.img`

---

## Group 4 — "cdvdstm era" · ioprp260–310 (v0x0220–v0x0226)

| Criterion | Value |
|-----------|-------|
| sceCdMmode | works |
| sceCdRead no-disc | returns 0, err=254 |
| Concurrent reads | rejected |
| Streaming | **moved to cdvdstm.irx** — cdvdman stubs return 0 |
| sceCdRE (114) | **implemented** |
| sceCdReadCdda (40) | ioprp280+ only |
| sceCdCtrlADout (45) | ioprp280+ only |
| cdvdman exports | 123–191 slots |
| cdvdstm.irx needed | **Yes** |

**Internal split:**
- **4a — ioprp260–271a/280a (v0x0220–v0x0223):** no audio CD (cdvdstm variant A only — no sceCdReadCdda)
- **4b — ioprp271b/280b–310b (v0x0222–v0x0226):** cdvdstm variant B/C — includes sceCdReadCdda in cdvdstm
- **4c — ioprp271a/280a/300a/310a:** cdvdstm variant A — DVD-only streaming

**Neutrino concern:** cdvdman_emu must **not** implement sceCdSt* functions for this group — games using these versions call sceCdStInit from cdvdstm.irx, not cdvdman. Neutrino must load the appropriate cdvdstm variant alongside cdvdman. The a/b split is hardware-driven: fat PS2 needs the "b" (sceCdReadCdda-capable) variant.

**Representatives:** `ioprp_260.img` + `cdvdstm_260.irx`, `ioprp_280.img` + `cdvdstm_280b.irx`, `ioprp_310.img` + `cdvdstm_310b.irx`

---

## Summary Table

| Group | IOPRP | cdvdman ver | mmode | no-disc err | Streaming location | Streaming era | sceCdRE | cdvdstm |
|-------|-------|-------------|-------|-------------|-------------------|---------------|---------|---------|
| 0     | bios/14    | ≤0x0106        | No (18) | 1+callback   | cdvdman | Era 0 (continuous) | No  | No        |
| 1     | 15–16      | 0x0207–020a    | No (18) | 1+callback   | cdvdman | Era 1-ish          | No  | No        |
| 2a    | 165        | 0x020b         | Yes     | 0, err=0     | cdvdman | Era 1 (no StStop cb) | No | No       |
| 2b    | 202–214    | 0x020d–0210    | Yes     | 0, err=0     | cdvdman | Era 2 (+reason=8)  | No  | No        |
| 3a    | 224–23     | 0x0214–0215    | Yes     | 0, err=254   | cdvdman | Era 2              | No  | No        |
| 3b    | 234–243    | 0x0216–021a    | Yes     | 0, err=254   | cdvdman | Era 3 (cb=bankmax) | No  | No        |
| 3c    | 250–255    | 0x021c–021d    | Yes     | 0, err=254   | cdvdman | Era 3              | No  | No        |
| 4a    | 260–280a   | 0x0220–0223    | Yes     | 0, err=254   | cdvdstm | Era 4 (stub)       | Yes | A (no Cdda) |
| 4b    | 271b–310   | 0x0222–0226    | Yes     | 0, err=254   | cdvdstm | Era 4 (stub)       | Yes | B/C (Cdda) |

---

## Key Neutrino Takeaways

1. **Minimum viable target is Group 3b/3c** — Era 3 streaming (cb_count=bankmax), mmode=1, sceCdRead fail with err=254. Most retail games were written for ioprp234–255.
2. **Streaming era in cdvdman_emu must match the loaded IOPRP** — if neutrino loads ioprp260+, sceCdSt* must stub to 0 and cdvdstm.irx must be loaded instead.
3. **The callback always fires from interrupt context** (Finding 1, confirmed all versions) — cdvdman_emu dispatching from a thread is wrong for every group.
4. **Groups 0 and 1 can share one emulation path** (no-mmode, accept-concurrent-reads), Group 2+ another, Group 4 requires the cdvdstm split.
5. **Test matrix:** one representative per group is sufficient — `ioprp_14.img`, `ioprp_165.img`, `ioprp_202.img`, `ioprp_253.img`, `ioprp_260.img`+cdvdstm, `ioprp_310.img`+cdvdstm.
