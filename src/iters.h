#pragma once

#include "def.h"
#include "seq.h"

typedef struct {
    const seq_vt *seq_vt;
    const seq_vt *inner_vt;
    void *inner;
    bool (*pred)(void *);
} iter_filter;

static void *_iter_filter_next(iter_filter *self) {
    void *item;
    while ((item = seq_next(self->inner_vt, self->inner))) {
        if (self->pred(item)) {
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

static iter_filter iter_filter_new(const seq_vt *inner_vt, void *inner, bool (*pred)(void *)) {
    return (iter_filter){
        .seq_vt = &ITER_FILTER_DYN_SEQ,
        .inner_vt = inner_vt,
        .inner = inner,
        .pred = pred,
    };
}

#define FILTER(iter, pred) (iter_filter_new((iter).seq_vt, &(iter), (bool (*)(void *))(pred)))
