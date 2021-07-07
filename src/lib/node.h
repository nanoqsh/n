#pragma once

#include "../def.h"
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

static void node_for_each(node self, hof on_item) {
    for (node curr = self; curr; curr = *node_tail(curr)) {
        void *value = node_get(curr);
        hof_call(on_item, value);
    }
}

static void node_drop_with(node self, hof on_item) {
    node next;
    for (node curr = self; curr; curr = next) {
        void *value = node_get(curr);
        hof_call(on_item, value);

        next = *node_tail(curr);
        free(curr);
    }
}

static void node_drop(node self) { node_drop_with(self, hof_empty()); }

static word node_count(node self) {
    word counter = 0;
    for (node curr = self; curr; curr = *node_tail(curr)) {
        ++counter;
    }

    return counter;
}

static node node_last(node self) {
    if (!self) {
        return NULL;
    }

    node curr = self;
    for (node next; (next = *node_tail(curr)); curr = next) {
        //
    }
    return curr;
}

// Connects node `a` with node `b` and returns tail of `a`.
// `b` node may be NULL.
static node node_connect(node a, node b) {
    node *tail_ptr = node_tail(a);
    node tail = *tail_ptr;
    *tail_ptr = b;
    return tail;
}

static node node_push_front(node self, fptr val) {
    node n = node_new(val);
    node_connect(n, self);
    return n;
}

static node node_push_back(node self, fptr val) {
    node n = node_new(val);
    if (!self) {
        return n;
    }

    node last = node_last(self);
    node_connect(last, n);
    return self;
}

static node node_find_with(node self, hof pred) {
    for (node curr = self; curr; curr = *node_tail(curr)) {
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
