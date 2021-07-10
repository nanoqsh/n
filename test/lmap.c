#include "test.h"

#include "../src/def.h"
#include "../src/lib/lmap.h"

static u64 hash_u32(const u32 *val) {
    u32 x = *val;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

static void new_drop() {
    lmap m = lmap_new();

    ASSERT(lmap_table_size(&m) == 0);
    ASSERT(lmap_len(&m) == 0);
    ASSERT(lmap_load_factor(&m) == 0.);

    lmap_drop(&m);
}

static void with_pow() {
    lmap m = lmap_with_pow(3);

    ASSERT(lmap_table_size(&m) == 8);
    ASSERT(lmap_len(&m) == 0);
    ASSERT(lmap_load_factor(&m) == 0.);

    lmap_drop(&m);
}

static void drop_fn(const entry_ptr *a, const entry_ptr *b) {
    ASSERT(*(u32 *)a->key == *(u32 *)b->key);
    ASSERT(*(u32 *)a->val == *(u32 *)b->val);
}

static void drop_with() {
    lmap m = lmap_new();
    u32 key = 1;
    u32 val = 2;
    lmap_insert(&m, FPTR(u32, &key), FPTR(u32, &val), (hash_fn)hash_u32);

    entry_ptr pair = {
        .key = &key,
        .val = &val,
    };
    lmap_drop_with(&m, HOF_WITH(drop_fn, &pair), sizeof(u32));
}

static void for_each_fn(const entry_ptr *e) {
    u32 key = *(u32 *)e->key;
    u32 val = *(u32 *)e->val;
    ASSERT((key == 1 && val == 2) || (key == 3 && val == 4) || (key == 5 && val == 6));
}

static void insert_for_each() {
    lmap m = lmap_new();
    u32 keys[] = {1, 3, 5};
    u32 vals[] = {2, 4, 6};
    for (word i = 0; i < ARRAY_SIZE(keys); ++i) {
        u32 key = keys[i];
        u32 val = vals[i];
        bool inserted = lmap_insert(&m, FPTR(u32, &key), FPTR(u32, &val), (hash_fn)hash_u32);
        ASSERT(inserted);
    }

    lmap_for_each(&m, HOF(for_each_fn), sizeof(u32));

    lmap_drop(&m);
}

static void get_realloc() {
    u32 n = 12;
    lmap m = lmap_new();

    for (u32 i = 0; i < n; ++i) {
        bool inserted = lmap_insert(&m, FPTR(u32, &i), FPTR(u32, &i), (hash_fn)hash_u32);
        ASSERT(inserted);
    }
    ASSERT(lmap_len(&m) == n);

    for (u32 i = 0; i < n; ++i) {
        u32 *val = lmap_get(&m, FPTR(u32, &i), (hash_fn)hash_u32);
        ASSERT(*val == i);
    }

    lmap_realloc(&m, 4);
    for (u32 i = 0; i < n; ++i) {
        bool contains = (bool)lmap_get(&m, FPTR(u32, &i), (hash_fn)hash_u32);
        ASSERT(contains);
    }
    ASSERT(lmap_len(&m) == n);
    ASSERT(lmap_table_size(&m) == 16);

    lmap_drop(&m);
}

void test_lmap() {
    new_drop();
    with_pow();
    drop_with();
    insert_for_each();
    get_realloc();
}
