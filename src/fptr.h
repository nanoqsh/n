#pragma once

#include "def.h"

typedef struct {
    void *data;
    word size;
} fptr;

static fptr fptr_new(void *data, word size) {
    return (fptr){
        .data = data,
        .size = size,
    };
}

#define FPTR(type, data) (fptr_new((data), sizeof(type)))
