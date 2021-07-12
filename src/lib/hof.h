#pragma once

#include "nonnull.h"

typedef void *(*hof_ptr)(void *args, void *data);

typedef struct {
    hof_ptr fn;
    void *data;
} hof;

static hof hof_from_data(hof_ptr fn, void *data) {
    return (hof){
        .fn = fn,
        .data = data,
    };
}

#define HOF_WITH(fn, data) (hof_from_data((hof_ptr)(fn), (void *)(data)))

static hof hof_new(hof_ptr fn) { return hof_from_data(fn, NULL); }

#define HOF(fn) (hof_new((hof_ptr)(fn)))

static hof hof_empty() { return hof_new(NULL); }

static void *hof_call(hof self, void *args) {
    hof_ptr fn = self.fn;
    if (fn) {
        return fn(args, self.data);
    } else {
        return NULL;
    }
}

static void *_fn_not(void *args, hof *fn) { return BOOL_AS_PTR_NOT(hof_call(*fn, args)); }

static hof fn_not(hof *fn) { return HOF_WITH(_fn_not, fn); }

typedef struct {
    hof fn;
    bool val;
} fn_all_info;

static void *_fn_all(void *args, hof *fn) {
    bool result = hof_call(*fn, args);
    bool *val = (bool *)fn + 1;
    *val &= result;
    return BOOL_AS_PTR(result);
}

static fn_all_info fn_all(hof *fn) {
    return (fn_all_info){
        .fn = HOF_WITH(_fn_all, fn),
        .val = true,
    };
}

static bool fn_all_result(fn_all_info self) { return self.val; }
