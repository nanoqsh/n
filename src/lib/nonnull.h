#pragma once

#include "types.h"

static const word _GLOBAL_ZERO = 0;
#define NONNULL ((void *)&_GLOBAL_ZERO)
#define BOOL_AS_PTR(a) ((a) ? NONNULL : NULL)
#define BOOL_AS_PTR_NOT(a) ((a) ? NULL : NONNULL)
