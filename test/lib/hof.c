#include "../test.h"

#include "../../src/def.h"

static bool is_one(const u32 *val) { return *val == 1; }

static bool is_eq(const u32 *a, const u32 *b) { return *a == *b; }

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

static void not() {
    hof fn = HOF(is_one);
    u32 val = 1;
    hof nfn = HOF_WITH(_fn_not, &fn);
    ASSERT(!hof_call(nfn, &val));
}

void test_hof() {
    call();
    call_with();
    not();
}
