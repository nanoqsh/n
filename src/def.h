#pragma once

#include "lib/assert.h"
#include "lib/box.h"
#include "lib/col.h"
#include "lib/fptr.h"
#include "lib/hof.h"
#include "lib/range.h"
#include "lib/slice.h"
#include "lib/types.h"
#include "lib/vptr.h"
#include "lib/wbool.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define DYN(proto, type, obj) (vptr_new(obj, &VT_##type##_FOR_##proto))
