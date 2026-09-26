/*
  LUNA direct in-game return.

  LUNA modifications: Danny Nunez (dnunezx) 2026

  Pad discovery and hook code is adapted from Open PS2 Loader's padhook.c.
  Copyright 2009-2010 Ifcaro, jimmikaelkael and Polo.
  Licensed under Academic Free License version 3.0.
*/

#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <tamtypes.h>

#include "asm.h"
#include "cheat_api.h"
#include "ee_debug.h"
#include "eecore_config.h"
#include "gsm_api.h"
#include "iopmgr.h"
#include "luna_igr_rpc.h"
#include "padpatterns.h"
#include "padhook.h"
#include "ps2sdk_ee_regs.h"
#include "tlb.h"
#include "util.h"

#define DBGCOL(...) do { } while (0)
#define BGCOLND(...) do { } while (0)
#define PADHOOK 0

static int (*scePadPortOpen)(int port, int slot, void *addr);
static int (*scePad2CreateSocket)(pad2socketparam_t *SocketParam, void *addr);

static paddata_t Pad_Data;
static int IGR_Thread_ID = -1;
static int IGR_Intc_ID = -1;
int padOpen_hooked = 0;
static int EnableDebug = 0;

#define IGR_STACK_SIZE (4 * 1024)
static u8 IGR_Stack[IGR_STACK_SIZE] __attribute__((aligned(16)));

extern void *_gp;
extern void *_stack_end;

void DisableGSM(void);

static void luna_igr_fail(void)
{
    *GS_REG_BGCOLOR = COLOR_RED;
    SleepThread();
}

static void luna_return_home(void)
{
    char *argv[2];
    t_ExecData elf;

    SifInitRpc(0);

    // The HDD-chain target deliberately does not remount the game drive from
    // the resident IGR environment. Return through the standard PS2 browser
    // path so the console initializes the HDD and executes the installed
    // ROCKET MBR. GalaxyHDD maps the still-held START trigger back to LUNA.
    if (eec.ExitPath[0] == 'h' && eec.ExitPath[1] == 'd' && eec.ExitPath[2] == 'd' && eec.ExitPath[3] == '\0') {
        SifExitRpc();
        FlushCache(WRITEBACK_DCACHE);
        FlushCache(INVALIDATE_ICACHE);
        Exit(0);
        luna_igr_fail();
    }

    // Reset_Iop() leaves the EE-side RPC/load-file clients shut down. Rebuild
    // them before loading the memory-card modules and the launcher ELF.
    SifInitIopHeap();
    SifLoadFileInit();
    sbv_patch_enable_lmb();

    sbv_patch_disable_prefix_check();

    // A clean ROM IOP plus the standard memory-card modules is sufficient
    // to load LUNA without touching the exFAT game drive.
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
    SifLoadModule("rom0:MCSERV", 0, NULL);

    argv[0] = eec.ExitPath;
    argv[1] = NULL;

    // Preserve EE core and its active stack; discard resident module storage
    // and all game memory before loading the launcher.
    WipeUserMemory((void *)&_stack_end, (void *)GetMemorySize());
    FlushCache(WRITEBACK_DCACHE);
    FlushCache(INVALIDATE_ICACHE);

    if (SifLoadElf(argv[0], &elf) == 0) {
        SifExitIopHeap();
        SifLoadFileExit();
        SifExitRpc();

        FlushCache(WRITEBACK_DCACHE);
        FlushCache(INVALIDATE_ICACHE);
        ExecPS2((void *)elf.epc, (void *)elf.gp, 1, argv);
    }

    luna_igr_fail();
}

static void IGR_Thread(void *arg)
{
    u32 cop0_perf;
    int poweroff;

    (void)arg;
    SleepThread();

    SifInitRpc(0);

    poweroff = (Pad_Data.combo_type == IGR_COMBO_R3_L3);

    // Fail closed if the IOP cannot finish optical reads before the reset.
    // DEV9 stays powered for IGR and is shut down only for physical power-off.
    if (lunaIGRShutdown(poweroff) < 0)
        luna_igr_fail();

    // A stock power-button press powers the console off. If the command was
    // accepted but the hardware is still running, do not turn it into an IGR.
    if (poweroff)
        SleepThread();

    while (!Reset_Iop("", 0)) {
    }

    Remove_Kernel_Hooks();
    InitializeTLB();

    __asm__ __volatile__("mfc0 %0, $25" : "=r"(cop0_perf));
    if (cop0_perf & 0x80000000) {
        __asm__ __volatile__(
            "mfc0 $3, $25;"
            "lui $2, 0x8000;"
            "or $3, $3, $2;"
            "xor $3, $3, $2;"
            "mtc0 $3, $25;"
            "sync.p;");
    }

    if (eec.GsmVideoMode != EECORE_GSM_VMODE_NONE)
        DisableGSM();
    if (eec.CheatList != NULL)
        DisableCheats();

    while (!SifIopSync()) {
    }

    ExecPS2(luna_return_home, &_gp, 0, NULL);
    luna_igr_fail();
}

static int IGR_Intc_Handler(int cause)
{
    int i;
    int combo_pressed = 0;
    u8 pad_pos_state;
    u8 pad_pos_frame;
    u8 pad_pos_combo1;
    u8 pad_pos_combo2;

    (void)cause;

    if (Pad_Data.pad_buf != NULL) {
        pad_pos_state = ((u8 *)UNCACHED_SEG(Pad_Data.pad_buf))[Pad_Data.pos_state];
        pad_pos_frame = ((u8 *)UNCACHED_SEG(Pad_Data.pad_buf))[Pad_Data.pos_frame];
        pad_pos_combo1 = ((u8 *)UNCACHED_SEG(Pad_Data.pad_buf))[Pad_Data.pos_combo1];
        pad_pos_combo2 = ((u8 *)UNCACHED_SEG(Pad_Data.pad_buf))[Pad_Data.pos_combo2];

        if (((Pad_Data.libpad == IGR_LIBPAD) && (pad_pos_state == IGR_PAD_STABLE_V1)) ||
            ((Pad_Data.libpad == IGR_LIBPAD2) && (pad_pos_state == IGR_PAD_STABLE_V2))) {
            if (Pad_Data.vb_count++ >= 10) {
                padOpen_hooked = (pad_pos_frame != Pad_Data.prev_frame);
                Pad_Data.prev_frame = pad_pos_frame;
                Pad_Data.vb_count = 0;
            }

            combo_pressed = (pad_pos_combo1 == IGR_COMBO_R1_L1_R2_L2 &&
                             pad_pos_combo2 == IGR_COMBO_START_SELECT);
        }
    }

    // Act on the first detected frame. Some games use this same combination
    // for their own soft reset and can restart before a long hold expires.
    if (combo_pressed) {
        Pad_Data.combo_type = IGR_COMBO_START_SELECT;

        asm volatile("sync.l\n");

        u32 dmaEnableR = *R_EE_D_ENABLER;
        *R_EE_D_ENABLEW = dmaEnableR | 0x10000;
        *R_EE_D_CTRL;
        *R_EE_D_STAT;
        *R_EE_D0_CHCR = 0;
        *R_EE_D1_CHCR = 0;
        *R_EE_D2_CHCR = 0;
        *R_EE_D3_CHCR = 0;
        *R_EE_D4_CHCR = 0;
        *R_EE_D8_CHCR = 0;
        *R_EE_D9_CHCR = 0;
        *R_EE_D_ENABLEW = dmaEnableR;

        asm volatile("sync.l\n");
        *R_EE_GS_CSR = 0x100;
        asm volatile("sync.l\n");
        while (*R_EE_GS_CSR & 0x100) {
        }

        iResetEE(0x7F);

        for (i = 1; i < 256; i++) {
            if (i != IGR_Thread_ID)
                iSuspendThread(i);
        }

        iChangeThreadPriority(IGR_Thread_ID, 0);
        iWakeupThread(IGR_Thread_ID);
    }

    ExitHandler();
    return 0;
}

// Install_IGR() must be run first.
static void Set_libpad_Params(void *addr)
{
    DI();

    Pad_Data.pad_buf = addr;

    // Set positions of pad data and pad state in buffer
    if (Pad_Data.libpad == IGR_LIBPAD) {
        if (Pad_Data.libversion >= 0x0160) {
            Pad_Data.pos_combo1 = 3;
            Pad_Data.pos_combo2 = 2;
            Pad_Data.pos_state = 112;
            Pad_Data.pos_frame = 88;
        } else {
            Pad_Data.pos_combo1 = 11;
            Pad_Data.pos_combo2 = 10;
            Pad_Data.pos_state = 4;
            Pad_Data.pos_frame = 0;
        }
    } else if (Pad_Data.libpad == IGR_LIBPAD2) {
        Pad_Data.pos_combo1 = 29;
        Pad_Data.pos_combo2 = 28;
        Pad_Data.pos_state = 4;
        Pad_Data.pos_frame = 124;
    }

    EI();
}

// Install IGR thread, and Pad interrupt handler
void Install_IGR(void)
{
    ee_thread_t thread_param;

    Pad_Data.pad_buf = NULL;

    // Init runtime Pad_Data information
    Pad_Data.vb_count = 0;
    Pad_Data.combo_type = 0x00;
    Pad_Data.prev_frame = 0x00;

    // Do not install the IGR thread or interrupt handler more than once.
    if (IGR_Thread_ID < 0) {
        // Create and start IGR thread
        thread_param.gp_reg = &_gp;
        thread_param.func = IGR_Thread;
        thread_param.stack = (void *)IGR_Stack;
        thread_param.stack_size = IGR_STACK_SIZE;
        thread_param.initial_priority = 127;
        IGR_Thread_ID = CreateThread(&thread_param);

        StartThread(IGR_Thread_ID, NULL);
    }

    if (IGR_Intc_ID < 0) {
        // Create IGR interrupt handler
        IGR_Intc_ID = AddIntcHandler(kINTC_VBLANK_END, IGR_Intc_Handler, 0);
        EnableIntc(kINTC_VBLANK_END);
    }
}

void Reset_Padhook(void)
{
    IGR_Intc_ID = -1;
    IGR_Thread_ID = -1;
    padOpen_hooked = 0;
}

// Hook function for libpad scePadPortOpen
static int Hook_scePadPortOpen(int port, int slot, void *addr)
{
    int ret;

    // Make sure scePadPortOpen function is still available
    if (port == 0 && slot == 0) {
        DPRINTF("IGR: Hook_scePadPortOpen - padOpen hooking check...\n");
        Install_PadOpen_Hook(0x00100000, 0x01ff0000, PADOPEN_CHECK);
    }

    // Call original scePadPortOpen function
    ret = scePadPortOpen(port, slot, addr);

    // Install IGR with libpad1 parameters
    if (port == 0 && slot == 0) {
        DPRINTF("IGR: Hook_scePadPortOpen - installing IGR...\n");
        Install_IGR();
        Set_libpad_Params(addr);
    }

    return ret;
}

// Hook function for libpad2 scePad2CreateSocket
static int Hook_scePad2CreateSocket(pad2socketparam_t *SocketParam, void *addr)
{
    int ret;

    // Make sure scePad2CreateSocket function is still available
    if ((SocketParam == NULL) || (SocketParam->port == 0 && SocketParam->slot == 0))
        Install_PadOpen_Hook(0x00100000, 0x01ff0000, PADOPEN_CHECK);

    // Call original scePad2CreateSocket function
    ret = scePad2CreateSocket(SocketParam, addr);

    // Install IGR with libpad2 parameters
    if ((SocketParam == NULL) || (SocketParam->port == 0 && SocketParam->slot == 0)) {
        Install_IGR();
        Set_libpad_Params(addr);
    }

    return ret;
}

// This function patch the padOpen calls. (scePadPortOpen or scePad2CreateSocket)
int Install_PadOpen_Hook(u32 mem_start, u32 mem_end, int mode)
{
    u32 *ptr, *ptr2;
    u32 inst, fncall;
    u32 mem_size, mem_size2;
    u32 pattern[1], mask[1];
    int i, found, patched;

    pattern_t padopen_patterns[NB_PADOPEN_PATTERN] = {
        {padPortOpenpattern0, padPortOpenpattern0_mask, sizeof(padPortOpenpattern0), 1, 0x0211},
        {pad2CreateSocketpattern0, pad2CreateSocketpattern0_mask, sizeof(pad2CreateSocketpattern0), 2, 0x0200},
        {pad2CreateSocketpattern1, pad2CreateSocketpattern1_mask, sizeof(pad2CreateSocketpattern1), 2, 0x0200},
        {pad2CreateSocketpattern2, pad2CreateSocketpattern2_mask, sizeof(pad2CreateSocketpattern2), 2, 0x0200},
        {padPortOpenpattern1, padPortOpenpattern1_mask, sizeof(padPortOpenpattern1), 1, 0x0210},
        {padPortOpenpattern2, padPortOpenpattern2_mask, sizeof(padPortOpenpattern2), 1, 0x0160},
        {padPortOpenpattern3, padPortOpenpattern3_mask, sizeof(padPortOpenpattern3), 1, 0x0150}};

    found = 0;
    patched = 0;

    // Loop for each libpad version
    for (i = 0; i < NB_PADOPEN_PATTERN; i++) {
        ptr = (u32 *)mem_start;
        while (ptr) {
            // Purple while PadOpen pattern search
            if (EnableDebug)
                DBGCOL(0x800080, PADHOOK, "Searching PadOpen() pattern");

            mem_size = mem_end - (u32)ptr;

            // First try to locate the orginal libpad's PadOpen function
            ptr = find_pattern_with_mask(ptr, mem_size, padopen_patterns[i].pattern, padopen_patterns[i].mask, padopen_patterns[i].size);
            if (ptr) {
                DPRINTF("IGR: found padopen pattern%d at 0x%08x mode=%d\n", i, (int)ptr, mode);
                found = 1;

                // Green while PadOpen patches
                if (EnableDebug)
                    DBGCOL(0x008000, PADHOOK, "Patching PadOpen()");

                // Save original PadOpen function
                if (padopen_patterns[i].type == IGR_LIBPAD)
                    scePadPortOpen = (void *)ptr;
                else
                    scePad2CreateSocket = (void *)ptr;

                if (mode == PADOPEN_HOOK) {
                    // Generate generic instruction pattern & mask for a J/JAL to PadOpen()
                    // Use 000010 as the operation, to match both J & JAL.
                    inst = 0x08000000 | (0x03ffffff & ((u32)ptr >> 2));

                    // Ignore bit 26 for the mask because the jump type can be either J (000010) or JAL (000011)
                    pattern[0] = inst;
                    mask[0] = 0xfbffffff;

                    DPRINTF("IGR: searching opcode %08x witk mask %08x\n", (int)pattern[0], (int)mask[0]);

                    // Search & patch for calls to PadOpen
                    ptr2 = (u32 *)mem_start;
                    while (ptr2) {
                        mem_size2 = (u32)((u8 *)mem_end - (u8 *)ptr2);

                        ptr2 = find_pattern_with_mask(ptr2, mem_size2, pattern, mask, sizeof(pattern));
                        if (ptr2) {
                            DPRINTF("IGR: found padOpen call at 0x%08x\n", (int)ptr2);

                            patched = 1;

                            fncall = (u32)ptr2;

                            // Get PadOpen call Jump Instruction type (JAL or J).
                            inst = (ptr2[0] & 0xfc000000);

                            // Get Hook_PadOpen call Instruction code
                            if (padopen_patterns[i].type == IGR_LIBPAD) {
                                DPRINTF("IGR: Hook_scePadPortOpen addr 0x%08x\n", (int)Hook_scePadPortOpen);
                                inst |= 0x03ffffff & ((u32)Hook_scePadPortOpen >> 2);
                            } else {
                                DPRINTF("IGR: Hook_scePad2CreateSocket addr 0x%08x\n", (int)Hook_scePad2CreateSocket);
                                inst |= 0x03ffffff & ((u32)Hook_scePad2CreateSocket >> 2);
                            }

                            DPRINTF("IGR: patching padopen call at addr 0x%08x with opcode %08x\n", (int)fncall, (int)inst);
                            // Overwrite the original PadOpen function call with our function call
                            _sw(inst, fncall);

                            Pad_Data.libpad = padopen_patterns[i].type;
                            Pad_Data.libversion = padopen_patterns[i].version;
                        }
                    }

                    // Locate pointers to scePadOpen(), likely used for JALR.
                    if (!patched) {
                        DPRINTF("IGR: 2nd padOpen patch attempt...\n");

                        // Make pattern with function address saved above
                        pattern[0] = (u32)ptr;
                        mask[0] = 0xffffffff;

                        DPRINTF("IGR: searching opcode %08x witk mask %08x\n", (int)pattern[0], (int)mask[0]);

                        // Search & patch for PadOpen function address
                        ptr2 = (u32 *)mem_start;
                        while (ptr2) {
                            mem_size2 = (u32)((u8 *)mem_end - (u8 *)ptr2);

                            ptr2 = find_pattern_with_mask(ptr2, mem_size2, pattern, mask, sizeof(pattern));
                            if (ptr2) {
                                DPRINTF("IGR: found padOpen call at 0x%08x\n", (int)ptr2);

                                patched = 1;

                                fncall = (u32)ptr2;

                                // Get Hook_PadOpen function address
                                if (padopen_patterns[i].type == IGR_LIBPAD) {
                                    DPRINTF("IGR: Hook_scePadPortOpen addr 0x%08x\n", (int)Hook_scePadPortOpen);
                                    inst = (u32)Hook_scePadPortOpen;
                                } else {
                                    DPRINTF("IGR: Hook_scePad2CreateSocket addr 0x%08x\n", (int)Hook_scePad2CreateSocket);
                                    inst = (u32)Hook_scePad2CreateSocket;
                                }

                                DPRINTF("IGR: patching padopen call at addr 0x%08x with opcode %08x\n", (int)fncall, (int)inst);
                                // Overwrite the original PadOpen function address with our function address
                                _sw(inst, fncall);

                                Pad_Data.libpad = padopen_patterns[i].type;
                                Pad_Data.libversion = padopen_patterns[i].version;
                            }
                        }
                    }
                } else {
                    DPRINTF("IGR: no hooking requested, breaking loop...\n");
                    // Hooking is not required and padOpen function was found, so stop searching
                    break;
                }

                // Increment search pointer
                // ptr += padopen_patterns[i].size;
                ptr += (padopen_patterns[i].size >> 2);
            }
        }

        // If a padOpen function call was patched or ( hooking is not required and a padOpen function was found ), so stop the libpad version search loop
        if (patched == 1 || (mode == PADOPEN_CHECK && found == 1)) {
            DPRINTF("IGR: job done exiting...\n");
            break;
        }
    }

    // Done
    if (EnableDebug)
        BGCOLND(0x000000); // Black

    return patched;
}
