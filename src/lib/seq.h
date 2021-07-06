#pragma once

#include "../def.h"

typedef void *(*seq_next_fn)(void *);
typedef void (*seq_drop_fn)(void *);

typedef struct {
    seq_next_fn next;
    seq_drop_fn drop;
} seq_vt;

static void *seq_next(const seq_vt *seq, void *self) { return seq->next(self); }

static void seq_drop(const seq_vt *seq, void *self) { seq->drop(self); }
