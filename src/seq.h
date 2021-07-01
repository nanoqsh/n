#pragma once

#include "def.h"

typedef struct {
    void *(*next)(void *);
    void (*drop)(void *);
} seq_vt;

static void *seq_next(const seq_vt *seq, void *self) { return seq->next(self); }

static void seq_drop(const seq_vt *seq, void *self) { seq->drop(self); }
