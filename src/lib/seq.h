#pragma once

#include "../def.h"

typedef void *(*seq_next_fn)(void *);
typedef void (*seq_drop_fn)(void *);

typedef struct {
    seq_next_fn next;
    seq_drop_fn drop;
} seq_vt;

static void *seq_next(vptr self) { return ((seq_vt *)self.vt)->next(self.data); }

static void seq_drop(vptr self) { ((seq_vt *)self.vt)->drop(self.data); }
