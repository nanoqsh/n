#include "test.h"

#include "../def.h"
#include "../lib/node.h"

static void new_drop() {
    u32 val = 12;
    node n = node_new(FPTR(u32, &val));

    ASSERT(*node_tail(n) == NULL);
    ASSERT(*(u32 *)node_get(n) == 12);
    ASSERT(node_count(n) == 1);

    node_drop(n);
}

static void drop_fn(const u32 *val) { ASSERT(*val == 12); }

static void drop_with() {
    u32 val = 12;
    node n = node_new(FPTR(u32, &val));

    ASSERT(*node_tail(n) == NULL);
    ASSERT(*(u32 *)node_get(n) == 12);
    ASSERT(node_count(n) == 1);

    node_drop_with(n, HOF(drop_fn));
}

static void connect_count() {
    u32 val = 12;
    node a = node_new(FPTR(u32, &val));
    node b = node_new(FPTR(u32, &val));
    node c = node_new(FPTR(u32, &val));

    ASSERT(node_connect(a, b) == NULL);
    ASSERT(node_count(a) == 2);
    ASSERT(node_count(b) == 1);
    ASSERT(node_count(c) == 1);

    ASSERT(node_connect(b, c) == NULL);
    ASSERT(node_count(a) == 3);
    ASSERT(node_count(b) == 2);
    ASSERT(node_count(c) == 1);

    node_drop(a);
}

static void for_each_fn(const u32 *val, u32 *counter) {
    --*counter;
    ASSERT(*val == *counter);
}

static void push_front_for_each() {
    u32 c = 12;
    node n = NULL;
    for (u32 i = 0; i < c; ++i) {
        n = node_push_front(n, FPTR(u32, &i));
    }
    ASSERT(node_count(n) == (word)c);

    u32 counter = c;
    node_for_each(n, HOF_WITH(for_each_fn, &counter));
    ASSERT(counter == 0);

    node_drop(n);
}

static void back_fn(const u32 *val, u32 *counter) {
    ASSERT(*val == *counter);
    ++*counter;
}

static void push_back() {
    u32 c = 12;
    node n = NULL;
    for (u32 i = 0; i < c; ++i) {
        n = node_push_back(n, FPTR(u32, &i));
    }
    ASSERT(node_count(n) == (word)c);

    u32 counter = 0;
    node_for_each(n, HOF_WITH(back_fn, &counter));
    ASSERT(counter == c);

    node_drop(n);
}

static void find() {
    u32 c = 12;
    node n = NULL;
    for (u32 i = 0; i < c; ++i) {
        n = node_push_front(n, FPTR(u32, &i));
    }
    ASSERT(node_count(n) == (word)c);

    for (u32 i = 0; i < c; ++i) {
        node found = node_find(n, FPTR(u32, &i));
        ASSERT(found);
        u32 *val = node_get(found);
        ASSERT(*val == i);
    }

    node found = node_find(n, FPTR(u32, &c));
    ASSERT(!found);

    node_drop(n);
}

void test_node() {
    new_drop();
    drop_with();
    connect_count();
    push_front_for_each();
    push_back();
    find();
}
