#pragma once

#include "def.h"

typedef void (*hof_ptr)(void *, void *);

typedef struct {
    hof_ptr fn;
    void *data;
} hof;

static void hof_call(hof *self, void *args) { (*self->fn)(args, self->data); }
