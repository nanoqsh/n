#pragma once

#include "../def.h"

static bool conv_str_to_num(slice str, u8 base, u64 *out) {
    u64 base_pow = 1;
    u8 *p = slice_end(str);
    while (p != slice_start(str)) {
        u8 c = *--p;
        u8 d;
        if (c >= '0' && c <= '9') {
            d = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            d = c - 'A' + 10;
        } else if (c == '_') {
            continue;
        } else {
            UNREACHABLE;
        }

        u64 n = d * base_pow;
        if (n / base_pow != d) {
            return false;
        }

        *out += n;
        if (*out < n) {
            return false;
        }

        u64 pow = base_pow * base;
        if (pow / base != base_pow) {
            return p == str.start;
        }
        base_pow = pow;
    }

    return true;
}
