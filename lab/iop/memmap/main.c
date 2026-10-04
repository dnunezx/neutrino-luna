/*
 * memmap.irx — IOP heap layout scanner
 *
 * Walks the sysmem allocator's internal block list and prints every block
 * (used and free) with its address, size, and attribution.
 *
 * Attribution logic for USED blocks:
 *   img  — block contains a loaded module's image (text+data+bss).
 *           Detected by finding a ModuleInfo_t whose text_start falls
 *           within [block_start, block_end).
 *   heap — dynamic heap allocation or thread stack with no further identification.
 *
 * Usage:  SifLoadModule("cdrom0:\\MEMMAP.IRX;1", label_len, label)
 *   label — optional IOPRP version string printed in the header (e.g. "v2.60")
 *
 * Output goes to IOP TTY (visible in PCSX2's IOP Console).
 *
 * sysmem_alloc_element_t.info bit layout (from ps2sdk/sysmem.c):
 *   bits[31:17]  size in 256-byte units
 *   bits[15:1]   block start / 256
 *   bit[0]       1 = USED, 0 = FREE
 */

#include <loadcore.h>
#include <sysmem.h>
#include <stdio.h>
#include <sysclib.h>

#define MODNAME "memmap"
IRX_ID(MODNAME, 1, 1);

/*
 * Find the module whose text_start falls within [start, end).
 * This correctly identifies module IMAGE blocks, which start 0x30 bytes
 * before text_start (the IRX header), so block_start < text_start.
 */
static const ModuleInfo_t *find_module_by_image(unsigned int start,
                                                  unsigned int end,
                                                  const lc_internals_t *lc)
{
    const ModuleInfo_t *mi = lc->image_info;
    while (mi) {
        unsigned int ts = mi->text_start;
        if (ts >= start && ts < end)
            return mi;
        mi = mi->next;
    }
    return NULL;
}

int _start(int argc, char **argv)
{
    const char *label = (argc > 1 && argv[1] && argv[1][0]) ? argv[1] : "?";

    sysmem_internals_t *sm = GetSysmemInternalData();
    if (!sm || !sm->alloclist) {
        printf("[MEMMAP] sysmem not available\n");
        return MODULE_NO_RESIDENT_END;
    }

    lc_internals_t *lc = GetLoadcoreInternalData();

    printf("[MEMMAP_BEGIN: %s]\n", label);
    printf("[MEMMAP]   start    end      size      type   owner\n");
    printf("[MEMMAP]   -------------------------------------------------------\n");

    sysmem_alloc_element_t *elem = sm->alloclist->list;
    int total_used = 0, total_free = 0, n_used = 0, n_free = 0;

    while (elem) {
        unsigned int info       = elem->info;
        unsigned int size_units = info >> 17;

        if (size_units > 0) {
            unsigned int start = (unsigned int)((unsigned short)(info) >> 1) << 8;
            unsigned int size  = size_units << 8;
            unsigned int end   = start + size;
            int is_used        = (info & 1) != 0;

            if (is_used) {
                const char *type, *owner;
                char owner_buf[16];

                const ModuleInfo_t *mi = lc ? find_module_by_image(start, end, lc) : NULL;
                if (mi) {
                    type = "img  ";
                    strncpy(owner_buf, mi->name, 15);
                    owner_buf[15] = '\0';
                    owner = owner_buf;
                } else {
                    type  = "heap ";
                    owner = "?";
                }

                printf("[MEMMAP]   0x%05x  0x%05x  %7dB  %s  %s\n",
                    start, end, size, type, owner);
                total_used += size;
                n_used++;
            } else {
                printf("[MEMMAP]   0x%05x  0x%05x  %7dB  free\n",
                    start, end, size);
                total_free += size;
                n_free++;
            }
        }

        elem = elem->next;
    }

    printf("[MEMMAP]   -------------------------------------------------------\n");
    printf("[MEMMAP]   used=%dB (%dKiB, %d blocks)  free=%dB (%dKiB, %d blocks)\n",
        total_used, total_used / 1024, n_used,
        total_free, total_free / 1024, n_free);
    printf("[MEMMAP_END: %s]\n", label);

    return MODULE_NO_RESIDENT_END;
}
