#include "../test.h"

#include "../../src/def.h"
#include "../../src/lib/vec.h"

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

static void to_slice() {
    u32 n = 12;
    vec v = vec_new();

    for (u32 i = 0; i < n; ++i) {
        vec_push(&v, &i, sizeof(u32));
    }

    slice s = vec_slice(&v, sizeof(u32));
    ASSERT(slice_len(s, sizeof(u32)) == vec_len(&v));
    u32 counter = 0;
    for (u32 *p = slice_start(s); p != slice_end(s); ++p) {
        ASSERT(*p == counter);
        ++counter;
    }
    ASSERT(counter == n);

    s = vec_slice_from_range(&v, RANGE(2, 7), sizeof(u32));
    ASSERT(slice_len(s, sizeof(u32)) == 5);
    counter = 2;
    for (u32 *p = slice_start(s); p != slice_end(s); ++p) {
        ASSERT(*p == counter);
        ++counter;
    }
    ASSERT(counter == 7);

    vec_drop(&v);
}

static void push_top_get_pop() {
    u32 n = 12;
    vec v = vec_new();

    for (u32 i = 0; i < n; ++i) {
        vec_push(&v, &i, sizeof(u32));
        ASSERT(vec_len(&v) == i + 1);
        u32 *top = vec_top(&v, sizeof(u32));
        ASSERT(*top == i);
    }

    for (u32 i = 0; i < n; ++i) {
        u32 *val = vec_get(&v, i, sizeof(u32));
        ASSERT(*val == i);
    }

    for (u32 i = 0; i < n; ++i) {
        u32 *val = vec_pop(&v, sizeof(u32));
        ASSERT(*val == (n - i - 1));
    }
    ASSERT(vec_len(&v) == 0);

    vec_drop(&v);
}

void test_vec() {
    new_drop();
    with_cap_drop();
    drop_with();
    to_slice();
    push_top_get_pop();
}
