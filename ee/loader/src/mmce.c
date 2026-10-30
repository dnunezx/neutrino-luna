// LUNA 2026
#include <loadfile.h>
#include <string.h>
#include <unistd.h>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include "mmce.h"

extern unsigned char mmceman_irx[];
extern unsigned int size_mmceman_irx;
extern unsigned char sio2man_irx[];
extern unsigned int size_sio2man_irx;

unsigned int mmce_set_gameid(const char *gameid, const char *bsd, int enabled)
{
    if (!enabled || !gameid || !gameid[0] || strlen(gameid) > 249 ||
        (bsd && !strcmp(bsd, "mx4sio")))
        return 0;
    // QuickBoot can retain MX4SIO from LUNA even for an HDD/USB launch.
    if (SifSearchModuleByName("mx4sio") >= 0)
        return 0;
    if (SifSearchModuleByName("IOX/File_Manager_Rpc") < 0)
        return 0;

    // Called only after loading every runtime/config file. Switching the card
    // earlier can hide Neutrino itself when LUNA is installed on mc0:/mc1:.
    if (SifSearchModuleByName("mmceman") < 0) {
        int result;
        // Old ROM SIO2MAN versions lack the reset exports MMCEMAN needs.
        if (SifSearchModuleByName("sio2man") < 0) {
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
    for (int slot = 0; slot < 2; slot++) {
        device[4] = '0' + slot;
        if (fileXioDevctl(device, 0x01, NULL, 0, NULL, 0) < 0 ||
            fileXioDevctl(device, 0x08, (void *)gameid, strlen(gameid) + 1, NULL, 0) < 0)
            continue;
        switched |= 1u << slot;
        for (int poll = 0; poll < 150; poll++) {
            usleep(100000);
            int status = fileXioDevctl(device, 0x02, NULL, 0, NULL, 0);
            if (status < 0 || !(status & 1))
                break;
        }
    }
    return switched;
}
