#pragma once

#include "hof.h"
#include "slice.h"
#include "types.h"
#include <stdlib.h>

typedef struct {
    u8 *data;
    word len;
} box;

static box box_from_slice(slice s) {
    word len = slice_len_bytes(s);
    u8 *data;
    if (len == 0) {
        data = NULL;
    } else {
        data = malloc(len);
        memcpy(data, s.start, len);
    }

    return (box){
        .data = data,
        .len = len,
    };
}

#define BOX_FROM_ARRAY(a) (box_from_slice(SLICE(a, (a) + sizeof(a) / sizeof(*(a)))))

#define BOX_STR(s) (box_from_slice(SLICE_STR(s)))

#define BOX(p) (box_from_slice(SLICE(p, (p) + 1)))

static box box_empty() { return box_from_slice(slice_empty()); }

static void box_drop(box self) { free(self.data); }

static slice box_to_slice(box self) {
    void *start = self.data;
    void *end = self.data + self.len;
    return slice_new(start, end);
}

static void box_drop_array(box self, hof on_item, word size) {
    slice s = box_to_slice(self);
    slice_for_each(s, on_item, size);
    box_drop(self);
}

static void *box_data(box self) { return self.data; }

static word box_len(box self) { return self.len; }
