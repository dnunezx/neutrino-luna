// LUNA 2026
#include <loadfile.h>
#include <smod.h>
#include <stdint.h>
#include <string.h>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include "mmce.h"

extern unsigned char mmceman_irx[];
extern unsigned int size_mmceman_irx;
extern unsigned char sio2man_irx[];
extern unsigned int size_sio2man_irx;

static int mmce_module_loaded(const char *name)
{
    smod_mod_info_t info;
    // ROM LOADFILE does not provide SifSearchModuleByName's RPC, even with
    // the load-module-buffer patch. Inspect the resident IOP module list.
    return smod_get_mod_by_name(name, &info) > 0;
}

// Adapted from Warksmith's GameID implementation. The loader disables the
// extra timer services, so libc sleep calls cannot pace card-status polling.
static void mmce_wait_ms(unsigned int ms)
{
    uint32_t start, now;

    while (ms--) {
        __asm__ __volatile__("mfc0 %0, $9" : "=r"(start));
        do {
            __asm__ __volatile__("mfc0 %0, $9" : "=r"(now));
        } while ((uint32_t)(now - start) < 294912); // EE clock / 1000
    }
}

unsigned int mmce_set_gameid(const char *gameid, const char *bsd, int enabled,
                             const char *launcher_path, int igr_enabled)
{
    if (!enabled || !gameid || !gameid[0] || strlen(gameid) > 249 ||
        (bsd && !strcmp(bsd, "mx4sio")))
        return 0;
    // QuickBoot can retain MX4SIO from LUNA even for an HDD/USB launch.
    if (mmce_module_loaded("mx4sio"))
        return 0;
    if (!mmce_module_loaded("IOX/File_Manager_Rpc"))
        return 0;

    // Called only after loading every runtime/config file. Switching the card
    // earlier can hide Neutrino itself when LUNA is installed on mc0:/mc1:.
    if (!mmce_module_loaded("mmceman")) {
        int result;
        // Old ROM SIO2MAN versions lack the reset exports MMCEMAN needs.
        if (!mmce_module_loaded("sio2man")) {
            if (SifExecModuleBuffer(sio2man_irx, size_sio2man_irx, 0, NULL, &result) < 0 || result == 1)
                return 0;
        }
        if (SifExecModuleBuffer(mmceman_irx, size_mmceman_irx, 0, NULL, &result) < 0 || result == 1)
            return 0;
    }
    if (fileXioInit() < 0)
        return 0;

    unsigned int switched = 0;
    char device[] = "mmce0:";
    char launcher_card[] = "mc0:";
    for (int slot = 0; slot < 2; slot++) {
        device[4] = '0' + slot;
        launcher_card[2] = '0' + slot;
        // Without IGR the boot card cannot be restored before LUNA reloads.
        if (!igr_enabled && launcher_path && !strncmp(launcher_path, launcher_card, 4))
            continue;
        if (fileXioDevctl(device, 0x01, NULL, 0, NULL, 0) < 0 ||
            fileXioDevctl(device, 0x08, (void *)gameid, strlen(gameid) + 1, NULL, 0) < 0)
            continue;
        switched |= 1u << slot;
        for (int poll = 0; poll < 150; poll++) {
            mmce_wait_ms(100);
            int status = fileXioDevctl(device, 0x02, NULL, 0, NULL, 0);
            if (status < 0 || !(status & 1))
                break;
        }
    }
    return switched;
}
