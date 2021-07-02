#include "def.h"
#include "hof.h"
#include "iters.h"
#include "node.h"
#include "seq.h"
#include "vec.h"
#include <alloca.h>

void print_int(const int *i) { printf("%d\n", *i); }

bool is_even(const int *i) { return *i % 2 == 0; }

void doubler(int *i) { *i *= 2; }

void test_vec() {
    vec v = vec_new();
    for (int i = 0; i < 12; ++i) {
        vec_push(&v, &i, sizeof(int));
    }

    vec_iter iter = vec_to_iter(v, sizeof(int));
    void *buf = alloca(sizeof(int));
    iter_buffered buffered = BUFFERED(iter, buf, sizeof(int));
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
        n = node_new(&value1, sizeof(int));
        int actual = *(int *)node_get(n);
        ASSERT(actual == value1);
    }

    node_print(n);

    {
        node next = node_new(&value2, sizeof(int));
        int actual = *(int *)node_get(next);
        ASSERT(actual == value2);
        *node_next(n) = next;
    }

    node_print(n);

    hof h = {
        .fn = (hof_ptr)print_int,
        .data = NULL,
    };
    node_drop_with(n, h);
}

void test() {
    test_vec();
    test_node();
}
