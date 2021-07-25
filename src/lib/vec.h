#pragma once

#include "../def.h"

typedef struct {
    u8 *data;
    word cap;
    word len;
} vec;

static vec vec_with_cap(word cap, word size) {
    return (vec){
        .data = alloc_no_init(cap * size),
        .cap = cap,
        .len = 0,
    };
}

static vec vec_new() { return vec_with_cap(0, 0); }

static void vec_for_each(vec *self, hof on_item, word size) {
    void *end = self->data + self->len * size;
    for (u8 *item = self->data; item != end; item += size) {
        hof_call(on_item, item);
    }
}

static void vec_drop(vec *self) { DEL(self->data); }

static void vec_drop_with(vec *self, hof on_item, word size) {
    vec_for_each(self, on_item, size);
    vec_drop(self);
}

static void *vec_data(vec *self) { return self->data; }

static word vec_cap(vec *self) { return self->cap; }

static word vec_len(vec *self) { return self->len; }

static slice vec_slice_from_range(vec *self, range rng, word size) {
    u8 *start = self->data + rng.start * size;
    u8 *end = self->data + rng.end * size;
    return SLICE(start, end);
}

static slice vec_slice(vec *self, word size) {
    return vec_slice_from_range(self, RANGE(0, self->len), size);
}

static void *vec_get(vec *self, word i, word size) {
    DEBUG_ASSERT(i < self->len);
    return self->data + i * size;
}

static void *vec_top(vec *self, word size) {
    DEBUG_ASSERT(self->len != 0);
    return vec_get(self, self->len - 1, size);
}

static void vec_realloc(vec *self, word cap, word size) {
    DEBUG_ASSERT(cap != 0);
    self->data = alloc_realloc(self->data, cap * size);
    self->cap = cap;
}

static void vec_push(vec *self, void *value, word size) {
    if (self->len == self->cap) {
        vec_realloc(self, self->cap == 0 ? 4 : self->cap * 2, size);
    }

    word len = self->len;
    self->len = len + 1;
    void *next = vec_get(self, len, size);
    memcpy(next, value, size);
}

static void *vec_pop(vec *self, word size) {
    DEBUG_ASSERT(self->len != 0);
    word len = self->len - 1;
    void *item = vec_get(self, len, size);
    self->len = len;
    return item;
}
