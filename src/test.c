#include "def.h"
#include "fptr.h"
#include "hof.h"
#include "iters.h"
#include "lmap.h"
#include "node.h"
#include "seq.h"
#include "vec.h"

bool is_multiple(const int *i, const int *j) { return *i % *j == 0; }

void mul(int *i, const int *j) { *i *= *j; }

void test_vec() {
    vec v = NEW(vec,);
    for (int i = 0; i < 12; ++i) {
        vec_push(&v, &i, sizeof(int));
    }
    ASSERT(LEN(vec, &v) == 12);

    vec_iter iter = vec_to_iter(&v, sizeof(int));
    int buf;
    iter_buffered buffered = BUFFERED(iter, &buf, sizeof(int));
    int two = 2;
    iter_filter filter = FILTER(buffered, HOF_WITH(is_multiple, &two));
    iter_update update = UPDATE(filter, HOF_WITH(mul, &two));
    FOR_IN(int *, item, update, printf("%d\n", *item))

    DROP(vec, &v);
}

void test_node() {
    node n;
    int value1 = 12;
    int value2 = 7;

    {
        n = NEW(node, FPTR(int, &value1));
        int actual = *(int *)node_get(n);
        ASSERT(actual == value1);
    }

    {
        node next = NEW(node, FPTR(int, &value2));
        int actual = *(int *)node_get(next);
        ASSERT(actual == value2);
        *node_tail(n) = next;
    }

    int find = 7;
    node found = node_find(n, FPTR(int, &find));
    ASSERT(found == *node_tail(n));

    DROP(node, n);
}

u64 int_hash(int *val) {
    int x = *val;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

bool int_cmp(const int *a, const int *b) {
    bool res = *a == *b;
    ASSERT(res);
    return res;
}

void print_ints(const entry_ptr *pair) {
    int key = *(int *)pair->key;
    int val = *(int *)pair->val;
    printf("%d: %d\n", key, val);
}

void test_lmap() {
    lmap m = lmap_with_pow(3);
    ASSERT(lmap_len(&m) == 0);

    int key = 65;
    int val = 12;
    fptr fkey = FPTR(int, &key);
    fptr fval = FPTR(int, &val);
    bool res = lmap_insert(&m, fkey, fval, (hash_fn)int_hash);
    ASSERT(res);
    ASSERT(LEN(lmap, &m) == 1);

    res = lmap_insert_with_cmp(&m, fkey, fval, (hash_fn)int_hash, (cmp_fn)int_cmp);
    ASSERT(!res);
    ASSERT(LEN(lmap, &m) == 1);

    for (word i = 0; i < 12; ++i) {
        fptr fkey = FPTR(int, &i);
        fptr fval = FPTR(int, &i);
        res = lmap_insert_with_cmp(&m, fkey, fval, (hash_fn)int_hash, (cmp_fn)int_cmp);
        ASSERT(res);
        ASSERT(LEN(lmap, &m) == i + 2);
    }

    puts("--");
    DROP_WITH(lmap, &m, HOF(print_ints), sizeof(int));
}

void test() {
    test_vec();
    test_node();
    test_lmap();
}
