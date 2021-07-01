#pragma once

#include "def.h"

typedef u8 **node;

static node *node_next(node self) { return (node *)self; }

static void *node_get(node self) { return self + sizeof(word); }

static node node_new(void *value, word size) {
    node self = malloc(sizeof(word) + size);
    *self = NULL;
    memcpy(node_get(self), value, size);
    return self;
}

static void node_drop_with(node self, hof *on_item) {
    node next;
    for (node curr = self; curr != NULL; curr = next) {
        if (on_item) {
            void *value = node_get(curr);
            hof_call(on_item, value);
        }

        next = *node_next(curr);
        free(curr);
    }
}

static void node_drop(node self) { node_drop_with(self, NULL); }

static void node_print(node self) {
    for (node curr = self; curr != NULL; curr = *node_next(curr)) {
        printf("%p -> ", (void *)curr);
    }
    printf("null \n");
}
