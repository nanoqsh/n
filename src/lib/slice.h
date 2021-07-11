#pragma once

#include "types.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    void *start;
    void *end;
} slice;

static slice slice_new(void *start, void *end) {
    return (slice){
        .start = start,
        .end = end,
    };
}

#define SLICE(start, end) (slice_new((void *)(start), (void *)(end)))

static slice slice_empty() { return slice_new(NULL, NULL); }

static slice slice_from_str(const char *str) { return SLICE(str, str + strlen(str)); }

static word slice_len_bytes(const slice *self) { return (u8 *)self->end - (u8 *)self->start; }

static word slice_len(const slice *self, word size) { return slice_len_bytes(self) / size; }

static void slice_for_each(slice *self, hof on_item, word size) {
    for (u8 *item = self->start; item != self->end; item += size) {
        hof_call(on_item, item);
    }
}

static void slice_print(const slice *self, FILE *file) {
    int len = (int)slice_len_bytes(self);
    if (len != 0) {
        fprintf(file, "%.*s", len, (const char *)self->start);
    }
}
