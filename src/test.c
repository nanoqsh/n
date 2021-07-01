#include "def.h"
#include "hof.h"
#include "node.h"
#include "seq.h"
#include "vec.h"

void print_int(const int *i) { printf("%d\n", *i); }

void test_vec() {
    vec v = vec_new();
    for (int i = 0; i < 8; ++i) {
        vec_push(&v, &i, sizeof(int));
    }

    int top = *(int *)vec_top(&v, sizeof(int));
    ASSERT(top == 7);

    FOR_IN(int*, item, vec_to_seq(v, sizeof(int)), {
        printf("%d\n", *item);
    })

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
