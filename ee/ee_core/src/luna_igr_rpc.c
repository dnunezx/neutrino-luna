// Original LUNA code: Danny Nunez (dnunezx) 2026
#include <kernel.h>
#include <ps2lib_err.h>
#include <sifrpc.h>
#include <tamtypes.h>

#include "luna_igr_rpc.h"

int lunaIGRShutdown(int poweroff)
{
    SifRpcClientData_t client __attribute__((aligned(64)));
    s32 request __attribute__((aligned(64)));
    int result;

    client.server = NULL;
    for (int tries = 0; tries < 2000 && client.server == NULL; tries++) {
        result = SifBindRpc(&client, 0x80000598, 0);
        if (result < 0)
            return -E_SIF_RPC_BIND;
        if (client.server == NULL)
            nopdelay();
    }

    if (client.server == NULL)
        return -E_SIF_RPC_BIND;

    *(s32 *)UNCACHED_SEG(&request) = poweroff;
    if (SifCallRpc(&client, 1, SIF_RPC_M_NOWBDC, &request, sizeof(request), &request, sizeof(request), NULL, NULL) < 0)
        return -E_SIF_RPC_CALL;

    return (*(s32 *)UNCACHED_SEG(&request) == 1) ? 0 : -E_SIF_RPC_CALL;
}
