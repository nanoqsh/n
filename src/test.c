#include "def.h"
#include "hof.h"
#include "node.h"
#include "vec.h"

void print_int(const int *i) { printf("%d\n", *i); }

void test_vec() {
    vec v = vec_new();
    for (int i = 0; i < 12; ++i) {
        vec_push(&v, &i, sizeof(int));
    }

    int top = *(int *)vec_top(&v, sizeof(int));
    ASSERT(top == 11);

    hof h = {
        .fn = (hof_ptr)print_int,
        .data = NULL,
    };

    vec_pop_with(&v, &h, sizeof(int));
    vec_drop_with(&v, &h, sizeof(int));
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
    node_drop_with(n, &h);
}

void test() {
    test_vec();
    test_node();
}
