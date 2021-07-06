#include "test.h"

#include "../def.h"
#include "../lib/entry.h"

static void new_drop() {
    u32 hash = 1;
    u32 key = 2;
    u32 val = 3;
    entry *e = entry_new(hash, FPTR(u32, &key), FPTR(u32, &val));

    ASSERT(entry_hash(e) == hash);
    ASSERT(*(u32 *)entry_key(e) == key);
    ASSERT(*(u32 *)entry_val(e, sizeof(u32)) == val);
    entry_ptr pair = entry_pair(e, sizeof(u32));
    ASSERT(*(u32 *)pair.key == key);
    ASSERT(*(u32 *)pair.val == val);

    entry_drop(e);
}

static void drop_fn(const entry_ptr *a, const entry_ptr *b) {
    ASSERT(*(u32 *)a->key == *(u32 *)b->key);
    ASSERT(*(u32 *)a->val == *(u32 *)b->val);
}

static void drop_with() {
    u64 hash = 1;
    u32 key = 2;
    u32 val = 3;
    entry *e = entry_new(hash, FPTR(u32, &key), FPTR(u32, &val));

    entry_ptr pair = {
        .key = &key,
        .val = &val,
    };
    entry_drop_with(e, HOF_WITH(drop_fn, &pair), sizeof(u32));
}

void test_entry() {
    new_drop();
    drop_with();
}
