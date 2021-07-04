#pragma once

#include "def.h"
#include "fptr.h"
#include "node.h"

typedef struct {
    u64 hash;
    // K key;
    // V val;
} entry;

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

static void entry_print(entry *self, void (*print_key_val)(const void *, const void *)) {
    u64 hash = entry_hash(self);
    printf("en: %zu ", hash);
    if (print_key_val) {
        const void *key = entry_key(self);
        const void *val = entry_val(self, sizeof(int));
        print_key_val(key, val);
    }
}