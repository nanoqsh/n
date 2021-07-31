#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef signed char i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t word;
typedef ssize_t iword;

typedef u64 (*hash_fn)(const void *);
typedef bool (*cmp_fn)(const void *, const void *);
