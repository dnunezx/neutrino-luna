#ifndef NEUTRINO_CHEAT_LIST_H
#define NEUTRINO_CHEAT_LIST_H
#include <stdint.h>
#include <stddef.h>
#include "../../ee_core/include/cheat_engine.h"

// Match SetupCheats' hook classification, including continuation words.
static inline int cheat_list_valid(const uint32_t *words, size_t count) {
    size_t hooks = 0, codes = 0;
    int next_hook = 1;
    if ((count & 1) || count > MAX_CHEATLIST) return 0;
    for (size_t i = 0; i < count; i += 2) {
        uint32_t address = words[i];
        if (!address && !words[i + 1]) return 0;
        if ((address & 0xfe000000U) == 0x90000000U && next_hook) hooks++;
        else codes++;
        if (hooks > MAX_HOOKS || codes > MAX_CODES) return 0;
        next_hook = (address & 0xf0000000U) != 0x40000000U &&
                    (address & 0xf0000000U) != 0x30000000U;
    }
    return 1;
}

static inline int cheat_instructions_valid(const uint32_t *words, size_t count) {
    uint32_t codes[MAX_CODES * 2];
    size_t used = 0;
    int next_hook = 1;
    if (!cheat_list_valid(words, count)) return 0;
    for (size_t i = 0; i < count; i += 2) {
        uint32_t address = words[i];
        if ((address & 0xfe000000U) != 0x90000000U || !next_hook) {
            codes[used++] = address;
            codes[used++] = words[i + 1];
        }
        next_hook = (address & 0xf0000000U) != 0x40000000U &&
                    (address & 0xf0000000U) != 0x30000000U;
    }
    for (size_t i = 0; i < used; i += 2) {
        uint32_t address = codes[i];
        unsigned type = address >> 28;
        if (type == 8 || type == 9 || type == 10 || type == 11 || type == 15) return 0;
        if (type == 4 || type == 5 || type == 6 ||
            (type == 3 && (address & 0x00600000U) == 0x00400000U)) {
            i += 2;
            if (i >= used) return 0;
        } else if (type == 13 || type == 14) {
            unsigned following = type == 13 ? codes[i + 1] >> 24 : (address >> 16) & 0xff;
            if (!following) following = 1;
            if (following > (used - i - 2) / 2) return 0;
        }
    }
    return 1;
}

static inline int cheat_payload_decode(const char *text, uint32_t *words, size_t *count) {
    if (!text || text[0] != '1' || text[1] != ':') return -1;
    text += 2;
    size_t used = 0;
    while (*text) {
        if (used == MAX_CHEATLIST) return -1;
        uint32_t word = 0;
        for (int digit = 0; digit < 8; digit++) {
            char c = *text++;
            unsigned value;
            if (c >= '0' && c <= '9') value = c - '0';
            else if (c >= 'a' && c <= 'f') value = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') value = c - 'A' + 10;
            else return -1;
            word = (word << 4) | value;
        }
        words[used++] = word;
    }
    if (!used || !cheat_instructions_valid(words, used)) return -1;
    *count = used;
    return 0;
}
#endif
