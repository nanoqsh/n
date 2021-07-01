#pragma once

#include "def.h"
#include "hof.h"

typedef struct {
    // self* -> T*
    hof next;

    // self*
    hof drop;
} seq_vt;

static seq_vt seq_new(hof next, hof drop) {
    return (seq_vt){
        .next = next,
        .drop = drop,
    };
}

static seq_vt seq_from_obj(void *self) { return *(seq_vt *)self; }

static void *seq_next(seq_vt *seq, void *self) { return hof_call(seq->next, self); }

static void seq_drop(seq_vt *seq, void *self) { hof_call(seq->drop, self); }
