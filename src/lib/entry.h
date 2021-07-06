#pragma once

#include "../def.h"
#include "node.h"

typedef struct {
    u64 hash;
    // K key;
    // V val;
} entry;

typedef struct {
    void *key;
    void *val;
} entry_ptr;

static entry *entry_new(u64 hash, fptr key, fptr val) {
    word size = sizeof(entry) + key.size + val.size;
    entry *data = malloc(size);
    data->hash = hash;
    u8 *ptr = (u8 *)(data + 1);
    memcpy(ptr, key.data, key.size);
    ptr += key.size;
    memcpy(ptr, val.data, val.size);
    return data;
}

static u64 entry_hash(const entry *self) { return self->hash; }

static void *entry_key(entry *self) { return self + 1; }

static void *entry_val(entry *self, word key_size) { return (u8 *)(self + 1) + key_size; }

static entry_ptr entry_pair(entry *self, word key_size) {
    return (entry_ptr){
        .key = entry_key(self),
        .val = entry_val(self, key_size),
    };
}

static void entry_drop(entry *self) { free(self); }

static void entry_drop_with(entry *self, hof on_item, word key_size) {
    entry_ptr pair = entry_pair(self, key_size);
    hof_call(on_item, &pair);
    entry_drop(self);
}
