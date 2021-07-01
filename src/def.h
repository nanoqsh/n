#pragma once

#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NONE ((void)0)

void assertion_failed(const char *a, const char *file, unsigned int line) {
    fprintf(stderr, "assertion failed: %s in file %s at line %d\n", a, file, line);
    exit(1);
}

#define ASSERT(a) ((a) ? NONE : assertion_failed(#a, (const char *)__FILE__, __LINE__))

#ifdef DEBUG
#define DEBUG_ASSERT(a) ASSERT(a)
#else
#define DEBUG_ASSERT(a) NONE
#endif

#define FOR_IN(type, item, seq, body)                                                              \
    {                                                                                              \
        __typeof(seq) _seq = (seq);                                                                \
        const seq_vt *_vt = _seq.vt;                                                               \
        type item;                                                                                 \
        while (((item) = seq_next(_vt, &_seq)))                                                    \
            body;                                                                                  \
        seq_drop(_vt, &_seq);                                                                      \
    }
