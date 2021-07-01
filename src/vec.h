#pragma once

#include "def.h"
#include "hof.h"

typedef struct {
    u8 *data;
    word cap;
    word len;
} vec;

static vec vec_with_cap(word cap, word size) {
    return (vec){
        .data = cap == 0 ? NULL : malloc(cap * size),
        .cap = cap,
        .len = 0,
    };
}

static vec vec_new() { return vec_with_cap(0, 0); }

static void vec_drop(vec *self) { free(self->data); }

static void vec_drop_with(vec *self, hof *on_item, word size) {
    void *end = self->data + self->len * size;
    if (on_item) {
        for (u8 *item = self->data; item != end; item += size) {
            hof_call(on_item, item);
        }
    }

    vec_drop(self);
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
    self->data = cap == 0 ? NULL : realloc(self->data, cap * size);
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

static void vec_pop_with(vec *self, hof *on_item, word size) {
    void *item = vec_pop(self, size);
    if (on_item) {
        hof_call(on_item, item);
    }
}
