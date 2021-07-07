#pragma once

#include "../lib/types.h"

typedef struct {
    void *data;
    const void *vt;
} vptr;

static vptr vptr_new(void *data, const void *vt) {
    return (vptr){
        .data = data,
        .vt = vt,
    };
}

#define VPTR(vt, data) (vptr_new((data), (vt)))
