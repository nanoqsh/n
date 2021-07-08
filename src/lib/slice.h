#pragma once

#include "types.h"

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

#define SLICE(start, end) (slice_new(start, end))

static slice slice_empty() { return slice_new(NULL, NULL); }

static word slice_len_bytes(slice *self) { return (u8 *)self->end - (u8 *)self->start; }

static word slice_len(slice *self, word size) { return slice_len_bytes(self) / size; }

static void slice_for_each(slice *self, hof on_item, word size) {
    for (u8 *item = self->start; item != self->end; item += size) {
        hof_call(on_item, item);
    }
}
