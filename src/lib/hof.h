#pragma once

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
