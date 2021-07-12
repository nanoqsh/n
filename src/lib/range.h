#pragma once

#include "types.h"

typedef struct {
    word start;
    word end;
} range;

static range range_new(word start, word end) {
    return (range){
        .start = start,
        .end = end,
    };
}

#define RANGE(start, end) (range_new(start, end))

static word range_len(range self) { return self.end - self.start; }
