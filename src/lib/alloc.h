#pragma once

#include "fptr.h"
#include <stdlib.h>
#include <string.h>

static u8 *alloc_no_init(word size) { return size == 0 ? NULL : malloc(size); }

static u8 *alloc_from(fptr val) {
    u8 *ptr = alloc_no_init(val.size);
    if (ptr) {
        memcpy(ptr, val.data, val.size);
    }
    return ptr;
}

#define ALC(type, ptr) ((type *)alloc_from(FPTR(type, ptr)))

static void alloc_del(const u8 *ptr) { free((void *)ptr); }

#define DEL(ptr) (alloc_del((const u8 *)(ptr)))

static u8 *alloc_realloc(const u8 *ptr, word size) {
    if (size == 0) {
        alloc_del(ptr);
        return NULL;
    }

    return realloc((void *)ptr, size);
}
