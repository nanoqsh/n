#pragma once

#include "def.h"
#include "entry.h"
#include "node.h"
#include <string.h>

typedef struct {
    node *table;
    word table_mask;
    word len;
} lmap;

static lmap _lmap_with_table_size(word size) {
    // Check size is power of 2 or 0
    DEBUG_ASSERT((size & (size - 1)) == 0);
    return (lmap){
        .table = size == 0 ? NULL : calloc(size, sizeof(void *)),
        .table_mask = size - 1,
        .len = 0,
    };
}

static lmap lmap_with_pow(word power_of_two) {
    word size = 1 << power_of_two;
    return _lmap_with_table_size(size);
}

static lmap lmap_new() { return _lmap_with_table_size(0); }

// drop

static word lmap_len(lmap *self) { return self->len; }

static word lmap_table_size(lmap *self) { return self->table_mask + 1; }

typedef struct {
    u64 hash;
    fptr key;
    cmp_fn cmp_fn;
} _lmap_find_key_info;

static bool _lmap_find_key(entry **en_ptr, _lmap_find_key_info *info) {
    entry *en = *en_ptr;
    if (en->hash != info->hash) {
        return false;
    }

    void *key = entry_key(en);
    if (info->cmp_fn) {
        return info->cmp_fn(key, info->key.data);
    } else {
        return memcmp(key, info->key.data, info->key.size) == 0;
    }
}

static bool lmap_insert_with_cmp(lmap *self, fptr key, fptr val, hash_fn hash_fn, cmp_fn cmp_fn) {
    // Get hash and table index
    u64 hash = hash_fn(key.data);
    word idx = (word)hash & self->table_mask;
    node *cell = self->table + idx;

    // Check is the key already in map
    _lmap_find_key_info info = {
        .hash = hash,
        .key = key,
        .cmp_fn = cmp_fn,
    };
    node found = node_find_with(*cell, HOF_WITH(_lmap_find_key, &info));
    if (found) {
        return false;
    }

    // If the key is new, insert it in list
    entry *new_entry = entry_new(hash, key, val);
    node new_node = node_new(FPTR(entry *, &new_entry));
    node tail = node_connect(new_node, *cell);
    DEBUG_ASSERT(tail == NULL);
    ++self->len;
    *cell = new_node;

    return true;
}

static bool lmap_insert(lmap *self, fptr key, fptr val, hash_fn hash_fn) {
    return lmap_insert_with_cmp(self, key, val, hash_fn, NULL);
}

static void lmap_print(lmap *self) {
    word size = lmap_table_size(self);
    for (word i = 0; i < size; ++i) {
        node n = self->table[i];
        node_print(n, NULL);
    }
}
