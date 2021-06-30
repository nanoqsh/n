#pragma once

#include "def.h"
#include "hof.h"

typedef struct {
    u8 *data;
    uw cap;
    uw len;
} vec;

static vec vec_with_cap(uw cap, uw size) {
    return (vec){
        .data = cap == 0 ? NULL : malloc(cap * size),
        .cap = cap,
        .len = 0,
    };
}

static vec vec_new() { return vec_with_cap(0, 0); }

static void vec_drop(vec *self) { free(self->data); }

static void vec_drop_with(vec *self, hof *on_item, uw size) {
    void *end = self->data + self->len * size;
    for (u8 *item = self->data; item != end; item += size) {
        hof_call(on_item, item);
    }

    vec_drop(self);
}

static void *vec_get(vec *self, uw i, uw size) { return self->data + i * size; }

static void vec_realloc(vec *self, uw cap, uw size) {
    self->data = cap == 0 ? NULL : realloc(self->data, cap * size);
    self->cap = cap;
}

static void vec_push(vec *self, void *value, uw size) {
    if (self->len == self->cap) {
        vec_realloc(self, self->cap == 0 ? 4 : self->cap * 2, size);
    }

    void *next = vec_get(self, self->len, size);
    self->len += 1;
    memcpy(next, value, size);
}

static void vec_pop(vec *self) { self->len -= 1; }

static void vec_pop_with(vec *self, hof *on_item, uw size) {
    vec_pop(self);
    void *item = vec_get(self, self->len, size);
    hof_call(on_item, item);
}
