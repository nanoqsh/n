#pragma once

#include "../def.h"
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
    lmap self;
    self.len = 0;

    if (size == 0) {
        self.table = NULL;
        self.table_mask = 0;
    } else {
        self.table = calloc(size, sizeof(void *));
        self.table_mask = size - 1;
    }

    return self;
}

static lmap lmap_with_pow(word power_of_two) {
    word size = 1 << power_of_two;
    return _lmap_with_table_size(size);
}

static lmap lmap_new() { return _lmap_with_table_size(0); }

static word lmap_len(lmap *self) { return self->len; }

static word lmap_table_size(lmap *self) {
    if (self->table) {
        return self->table_mask + 1;
    }

    return 0;
}

static float lmap_load_factor(lmap *self) {
    word table_size = lmap_table_size(self);
    if (table_size == 0) {
        return 0.;
    }

    float len = (float)lmap_len(self);
    return len / (float)table_size;
}

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

static void lmap_realloc(lmap *self, word power_of_two) {
    node *table = self->table;
    word size = lmap_table_size(self);
    word len = lmap_len(self);
    *self = lmap_with_pow(power_of_two);

    for (word i = 0; i < size; ++i) {
        node n = table[i];
        node next;
        for (node curr = n; curr != NULL; curr = next) {
            node *tail = node_tail(curr);
            next = *tail;
            *tail = NULL;

            const entry *en = *(entry **)node_get(curr);
            u64 hash = entry_hash(en);
            word idx = (word)hash & self->table_mask;
            node *cell = self->table + idx;

            node_connect(curr, *cell);
            *cell = curr;
        }
    }

    free(table);
    self->len = len;
}

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
    bool empty = !self->table;

    // If table is empty, create it
    if (empty) {
        *self = lmap_with_pow(3);
    }

    // Get hash and table index
    u64 hash = hash_fn(key.data);
    word idx = (word)hash & self->table_mask;
    node *cell = self->table + idx;

    if (!empty) {
        // Check is the key already in map
        _lmap_find_key_info info = {
            .hash = hash,
            .key = key,
            .cmp_fn = cmp_fn,
        };
        if (node_find_with(*cell, HOF_WITH(_lmap_find_key, &info))) {
            return false;
        }

        // Check load factor and realloc if necessary
        if (lmap_load_factor(self) > 3.) {
            word size = lmap_table_size(self);
            DEBUG_ASSERT(size != 0);
            word pow = 1;
            while (size >>= 1) {
                ++pow;
            }

            lmap_realloc(self, pow);
            idx = (word)hash & self->table_mask;
            cell = self->table + idx;
        }
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

static void *lmap_get_with_cmp(lmap *self, fptr key, hash_fn hash_fn, cmp_fn cmp_fn) {
    if (lmap_len(self) == 0) {
        return NULL;
    }

    u64 hash = hash_fn(key.data);
    word idx = (word)hash & self->table_mask;
    node *cell = self->table + idx;

    _lmap_find_key_info info = {
        .hash = hash,
        .key = key,
        .cmp_fn = cmp_fn,
    };
    node found = node_find_with(*cell, HOF_WITH(_lmap_find_key, &info));
    if (found) {
        entry *en = *(entry **)node_get(found);
        return entry_val(en, key.size);
    }

    return NULL;
}

static void *lmap_get(lmap *self, fptr key, hash_fn hash_fn) {
    return lmap_get_with_cmp(self, key, hash_fn, NULL);
}
