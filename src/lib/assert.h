#pragma once

#include <stdio.h>
#include <stdlib.h>

#define NONE ((void)0)

static void assertion_failed(const char *a, const char *file, unsigned int line) {
    fprintf(stderr, "assertion failed: %s in file %s at line %d\n", a, file, line);
    exit(1);
}

#define ASSERT(a) ((a) ? NONE : assertion_failed(#a, (const char *)__FILE__, __LINE__))

#define FAIL(a) (assertion_failed(#a, (const char *)__FILE__, __LINE__))

#define UNREACHABLE                                                                                \
    {                                                                                              \
        FAIL("unreachable code");                                                                  \
        exit(1);                                                                                   \
    }

#ifdef DEBUG
#define DEBUG_ASSERT(a) ASSERT(a)
#else
#define DEBUG_ASSERT(a) NONE
#endif
