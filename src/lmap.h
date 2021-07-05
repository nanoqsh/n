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

static word lmap_len(lmap *self) { return self->len; }

static word lmap_table_size(lmap *self) { return self->table_mask + 1; }

typedef struct {
    hof on_item;
    word key_size;
} _lmap_item_info;

static _lmap_item_info _lmap_item_info_new(hof on_item, word key_size) {
    return (_lmap_item_info){
        .on_item = on_item,
        .key_size = key_size,
    };
}

static void _lmap_on_entry(entry **en_ptr, _lmap_item_info *info) {
    entry *en = *en_ptr;
    entry_ptr pair = entry_pair(en, info->key_size);
    hof_call(info->on_item, &pair);
}

static void lmap_for_each(lmap *self, hof on_item, word key_size) {
    _lmap_item_info info = _lmap_item_info_new(on_item, key_size);
    word size = lmap_table_size(self);
    for (word i = 0; i < size; ++i) {
        node n = self->table[i];
        node_for_each(n, HOF_WITH(_lmap_on_entry, &info));
    }
}

static void _lmap_on_entry_drop(entry **en_ptr, _lmap_item_info *info) {
    entry *en = *en_ptr;
    entry_drop_with(en, info->on_item, info->key_size);
}

static void lmap_drop_with(lmap *self, hof on_item, word key_size) {
    _lmap_item_info info = _lmap_item_info_new(on_item, key_size);
    word size = lmap_table_size(self);
    for (word i = 0; i < size; ++i) {
        node n = self->table[i];
        node_drop_with(n, HOF_WITH(_lmap_on_entry_drop, &info));
    }
}

static void lmap_drop(lmap *self) { lmap_drop_with(self, hof_empty(), 0); }

typedef struct {
    u64 hash;
    fptr key;
    cmp_fn cmp_fn;
} _lmap_find_key_info;

static bool _lmap_find_key(entry **en_ptr, _lmap_find_key_info *info) {
    entry *en = *en_ptr;

    // First check the hash
    // If it not equals, then the key is not equals
    if (en->hash != info->hash) {
        return false;
    }

    // Compare keys if there is a `cmp_fn`
    // Otherwise compare bit by bit 
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
    if (node_find_with(*cell, HOF_WITH(_lmap_find_key, &info))) {
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
