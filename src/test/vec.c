#include "test.h"

#include "../def.h"
#include "../lib/vec.h"

static void new_drop() {
    vec v = vec_new();

    ASSERT(vec_data(&v) == NULL);
    ASSERT(vec_cap(&v) == 0);
    ASSERT(vec_len(&v) == 0);

    vec_drop(&v);
}

static void with_cap_drop() {
    vec v = vec_with_cap(6, sizeof(u32));

    ASSERT(vec_data(&v) != NULL);
    ASSERT(vec_cap(&v) == 6);
    ASSERT(vec_len(&v) == 0);

    vec_drop(&v);
}

static void drop_fn(const u32 *item, u32 *counter) {
    ASSERT(*item == *counter);
    ++*counter;
}

static void drop_with() {
    u32 n = 12;
    vec v = vec_new();

    for (u32 i = 0; i < n; ++i) {
        vec_push(&v, &i, sizeof(u32));
    }

    u32 counter = 0;
    vec_drop_with(&v, HOF_WITH(drop_fn, &counter), sizeof(u32));
    ASSERT(counter == n);
}

static void push_top_get_pop() {
    word n = 12;
    vec v = vec_new();

    for (word i = 0; i < n; ++i) {
        u32 val = (u32)i;
        vec_push(&v, &val, sizeof(u32));
        ASSERT(vec_len(&v) == i + 1);
        u32 *top = vec_top(&v, sizeof(u32));
        ASSERT(*top == val);
    }

    for (word i = 0; i < n; ++i) {
        u32 *val = vec_get(&v, i, sizeof(u32));
        ASSERT(*val == (u32)i);
    }

    for (word i = 0; i < n; ++i) {
        u32 *val = vec_pop(&v, sizeof(u32));
        ASSERT(*val == (u32)(n - i - 1));
    }
    ASSERT(vec_len(&v) == 0);

    vec_drop(&v);
}

void test_vec() {
    new_drop();
    with_cap_drop();
    drop_with();
    push_top_get_pop();
}
