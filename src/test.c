#include "def.h"
#include "fptr.h"
#include "hof.h"
#include "iters.h"
#include "lmap.h"
#include "node.h"
#include "seq.h"
#include "vec.h"

bool is_even(const int *i) { return *i % 2 == 0; }

void doubler(int *i) { *i *= 2; }

void test_vec() {
    vec v = vec_new();
    for (int i = 0; i < 12; ++i) {
        vec_push(&v, &i, sizeof(int));
    }

    vec_iter iter = vec_to_iter(&v, sizeof(int));
    int buf;
    iter_buffered buffered = BUFFERED(iter, &buf, sizeof(int));
    iter_filter filter = FILTER(buffered, is_even);
    iter_update update = UPDATE(filter, doubler);
    FOR_IN(int *, item, update, printf("%d\n", *item))

    vec_drop(&v);
}

void test_node() {
    node n;
    int value1 = 12;
    int value2 = 7;

    {
        n = node_new(FPTR(int, &value1));
        int actual = *(int *)node_get(n);
        ASSERT(actual == value1);
    }

    node_print(n);

    {
        node next = node_new(FPTR(int, &value2));
        int actual = *(int *)node_get(next);
        ASSERT(actual == value2);
        *node_tail(n) = next;
    }

    node_print(n);
    int find = 7;
    node found = node_find(n, FPTR(int, &find));
    ASSERT(found == *node_tail(n));

    node_drop(n);
}

u64 int_hash(int *val) {
    int x = *val;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

void test_lmap() {
    lmap m = lmap_with_pow(3);
    ASSERT(lmap_len(&m) == 0);

    int key = 7;
    int val = 12;
    lmap_insert(&m, FPTR(int, &key), FPTR(int, &val), (hash_fn)int_hash);
    ASSERT(lmap_len(&m) == 1);

    printf("--\n");
    lmap_print(&m);
}

void test() {
    test_vec();
    test_node();
    test_lmap();
}
