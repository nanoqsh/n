#pragma once

#include "types.h"

typedef void *wbool;

static const word _GLOBAL_ZERO = 0;
#define NONNULL ((void *)&_GLOBAL_ZERO)
#define BOOL_TO_WBOOL(a) ((a) ? NONNULL : NULL)
#define WBOOL_NOT(a) ((a) ? NULL : NONNULL)
