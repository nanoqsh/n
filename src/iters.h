#pragma once

#include "def.h"
#include "hof.h"
#include "seq.h"

typedef struct {
    const seq_vt *seq_vt;
    const seq_vt *inner_vt;
    void *inner;
    void *buf;
    word len;
} iter_buffered;

static void *_iter_buffered_next(iter_buffered *self) {
    void *item = seq_next(self->inner_vt, self->inner);
    if (item) {
        memcpy(self->buf, item, self->len);
        return self->buf;
    }

    return NULL;
}

static void _iter_buffered_drop(iter_buffered *self) { seq_drop(self->inner_vt, self->inner); }

const seq_vt ITER_BUFFERED_DYN_SEQ = {
    .next = (void *(*)(void *))_iter_buffered_next,
    .drop = (void (*)(void *))_iter_buffered_drop,
};

static iter_buffered iter_buffered_new(const seq_vt *inner_vt, void *inner, void *buf, word len) {
    return (iter_buffered){
        .seq_vt = &ITER_BUFFERED_DYN_SEQ,
        .inner_vt = inner_vt,
        .inner = inner,
        .buf = buf,
        .len = len,
    };
}

#define BUFFERED(iter, buf, len) (iter_buffered_new((iter).seq_vt, &(iter), (buf), (len)))

typedef struct {
    const seq_vt *seq_vt;
    const seq_vt *inner_vt;
    void *inner;
    // const T* -> bool
    hof pred;
} iter_filter;

static void *_iter_filter_next(iter_filter *self) {
    void *item;
    while ((item = seq_next(self->inner_vt, self->inner))) {
        if (hof_call(self->pred, item)) {
            return item;
        }
    }

    return NULL;
}

static void _iter_filter_drop(iter_filter *self) { seq_drop(self->inner_vt, self->inner); }

const seq_vt ITER_FILTER_DYN_SEQ = {
    .next = (void *(*)(void *))_iter_filter_next,
    .drop = (void (*)(void *))_iter_filter_drop,
};

static iter_filter iter_filter_new(const seq_vt *inner_vt, void *inner, hof pred) {
    return (iter_filter){
        .seq_vt = &ITER_FILTER_DYN_SEQ,
        .inner_vt = inner_vt,
        .inner = inner,
        .pred = pred,
    };
}

#define FILTER(iter, pred) (iter_filter_new((iter).seq_vt, &(iter), (pred)))

typedef struct {
    const seq_vt *seq_vt;
    const seq_vt *inner_vt;
    void *inner;
    // T* -> void
    hof update;
} iter_update;

static void *_iter_update_next(iter_update *self) {
    void *item = seq_next(self->inner_vt, self->inner);
    if (item) {
        hof_call(self->update, item);
        return item;
    }

    return NULL;
}

static void _iter_update_drop(iter_update *self) { seq_drop(self->inner_vt, self->inner); }

const seq_vt ITER_UPDATE_DYN_SEQ = {
    .next = (void *(*)(void *))_iter_update_next,
    .drop = (void (*)(void *))_iter_update_drop,
};

static iter_update iter_update_new(const seq_vt *inner_vt, void *inner, hof update) {
    return (iter_update){
        .seq_vt = &ITER_UPDATE_DYN_SEQ,
        .inner_vt = inner_vt,
        .inner = inner,
        .update = update,
    };
}

#define UPDATE(iter, update) (iter_update_new((iter).seq_vt, &(iter), (update)))
