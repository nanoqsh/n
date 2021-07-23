#include "../test.h"

#include "../../src/def.h"

static wbool is_one(const u32 *val) { return BOOL_TO_WBOOL(*val == 1); }

static wbool *is_eq(const u32 *a, const u32 *b) { return BOOL_TO_WBOOL(*a == *b); }

static void call() {
    hof fn = HOF(is_one);
    u32 val = 1;
    ASSERT(hof_call(fn, &val));
}

static void call_with() {
    u32 a = 1;
    u32 b = 1;
    hof fn = HOF_WITH(is_eq, &a);
    ASSERT(hof_call(fn, &b));
}

void test_hof() {
    call();
    call_with();
}
