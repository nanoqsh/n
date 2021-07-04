#pragma once

#include "def.h"
#include "fptr.h"
#include "hof.h"
#include <string.h>

// node -> node next
//         T value
//
typedef u8 **node;

static node *node_tail(node self) { return (node *)self; }

static void *node_get(node self) { return self + 1; }

static node node_new(fptr val) {
    node self = malloc(sizeof(node) + val.size);
    *self = NULL;
    memcpy(node_get(self), val.data, val.size);
    return self;
}

static void node_drop_with(node self, hof on_item) {
    node next;
    for (node curr = self; curr != NULL; curr = next) {
        void *value = node_get(curr);
        hof_call(on_item, value);

        next = *node_tail(curr);
        free(curr);
    }
}

static void node_drop(node self) { node_drop_with(self, hof_empty()); }

static void node_print(node self, void (*print_val)(const void *)) {
    for (node curr = self; curr != NULL; curr = *node_tail(curr)) {
        printf("%p -> ", (void *)curr);
        if (print_val) {
            const void *val = node_get(curr);
            print_val(val);
        }
    }
    printf("null \n");
}

// Connects node `a` with node `b` and returns tail of `a`.
// `b` node may be NULL.
static node node_connect(node a, node b) {
    node *tail_ptr = node_tail(a);
    node tail = *tail_ptr;
    *tail_ptr = b;
    return tail;
}

static node node_find_with(node self, hof pred) {
    for (node curr = self; curr != NULL; curr = *node_tail(curr)) {
        void *data = node_get(curr);
        if (hof_call(pred, data)) {
            return curr;
        }
    }

    return NULL;
}

static bool _node_cmp_val(void *data, fptr *val) { return memcmp(data, val->data, val->size) == 0; }

static node node_find(node self, fptr val) {
    return node_find_with(self, HOF_WITH(_node_cmp_val, &val));
}
