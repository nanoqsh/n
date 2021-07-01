#include "def.h"
#include "hof.h"
#include "vec.h"

void print_int(const int *i) { printf("%d\n", *i); }

void test() {
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
