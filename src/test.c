#include "def.h"
#include "hof.h"
#include "vec.h"

void print_int(const int *i) { printf("%d\n", *i); }

void assert_int(const int *a, const int *b) {
    if (*a != *b) {
        fprintf(stderr, "%d != %d\n", *a, *b);
        exit(1);
    }
}

void test() {
    vec v = vec_new();
    for (int i = 0; i < 12; ++i) {
        vec_push(&v, &i, sizeof(int));
    }

    vec_pop_with(
        &v,
        &(hof){
            .fn = (hof_ptr)assert_int,
            .data = &(int){11},
        },
        sizeof(int));

    vec_drop_with(
        &v,
        &(hof){
            .fn = (hof_ptr)print_int,
            .data = NULL,
        },
        sizeof(int));
}
