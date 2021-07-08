#pragma once

#include "lib/fptr.h"
#include "lib/hof.h"
#include "lib/range.h"
#include "lib/slice.h"
#include "lib/types.h"
#include "lib/vptr.h"
#include <stdio.h>
#include <string.h>

#define NONE ((void)0)

static void assertion_failed(const char *a, const char *file, unsigned int line) {
    fprintf(stderr, "assertion failed: %s in file %s at line %d\n", a, file, line);
    exit(1);
}

#define ASSERT(a) ((a) ? NONE : assertion_failed(#a, (const char *)__FILE__, __LINE__))

#ifdef DEBUG
#define DEBUG_ASSERT(a) ASSERT(a)
#else
#define DEBUG_ASSERT(a) NONE
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define DYN(proto, type, obj) (vptr_new(obj, &VT_##type##_FOR_##proto))
