#pragma once

#include "assert.h"
#include "hof.h"
#include "range.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    u8 *start;
    u8 *end;
} slice;

static slice slice_new(void *start, void *end) {
    return (slice){
        .start = start,
        .end = end,
    };
}

#define SLICE(start, end) (slice_new((void *)(start), (void *)(end)))

static slice slice_empty() { return slice_new(NULL, NULL); }

static slice slice_from_str(char *str) { return SLICE(str, str + strlen(str)); }

#define SLICE_STR(s) (slice_from_str((char *)(s)))

static word slice_len_bytes(slice self) { return self.end - self.start; }

static word slice_len(slice self, word size) { return slice_len_bytes(self) / size; }

static void slice_for_each(slice self, hof on_item, word size) {
    for (u8 *item = self.start; item != self.end; item += size) {
        hof_call(on_item, item);
    }
}

static void slice_print(slice self, FILE *file) {
    int len = (int)slice_len_bytes(self);
    if (len != 0) {
        fprintf(file, "%.*s", len, (const char *)self.start);
    }
}

static bool slice_cmp(slice self, slice rhs) {
    word len = slice_len_bytes(self);
    if (len != slice_len_bytes(rhs)) {
        return false;
    }

    return memcmp(self.start, rhs.start, len) == 0;
}

static bool slice_starts_with(slice self, slice sub) {
    word len = slice_len_bytes(sub);
    if (len > slice_len_bytes(self)) {
        return false;
    }

    return memcmp(self.start, sub.start, len) == 0;
}

static slice slice_subslice(slice self, range rng, word size) {
    DEBUG_ASSERT(range_len(rng) + rng.start <= slice_len(self, size));
    void *start = self.start + rng.start * size;
    void *end = self.start + rng.end * size;
    return SLICE(start, end);
}
